#include<algorithm>
#include<cstdint>
#include<fstream>
#include<iostream>
#include<set>
#include<sstream>
#include<unordered_map>
#include<utility>
#include"command_line_parser.h"
#include"common_types.h"
#include"configuration.h"
#include"instruction.h"
#include"instruction_parser.h"
#include"match_engine.h"
#include"order.h"
#include"order_matcher.h"

namespace match_engine
{
    namespace
    {
        const std::string ORDER_NOT_FOUND_ERROR("match_engine_error: Order not found");

        configuration _configuration;
        std::reference_wrapper<std::ostream> _logger_stream(std::cout);
        std::ifstream _instruction_file_stream;
        std::ofstream _output_file_stream;
        std::uintmax_t _last_timestamp(static_cast<std::uintmax_t>(0));
        std::uintmax_t _last_position(static_cast<std::uintmax_t>(0));

        std::ostream& logger_stream()
        {
            return _logger_stream;
        }

        bool log_error(const std::string& error)
        {
            if(!_configuration._ignore_errors)
            {
                logger_stream()<<error
                               <<'\n';
            }

            return _configuration._process_errors;
        }

        struct order_comparator
        {
            bool operator()(const order& left_comparate_order,
                            const order& right_comparate_order) const
            {
                if((left_comparate_order._side==order::side::BUY&&
                    left_comparate_order._price>right_comparate_order._price)||
                   (left_comparate_order._side==order::side::SELL&&
                    left_comparate_order._price<right_comparate_order._price))
                {
                    return true;
                }

                else if(common_types::decimal_equals(left_comparate_order._price,
                                                     right_comparate_order._price)&&
                        left_comparate_order._timestamp<right_comparate_order._timestamp)
                {
                    return true;
                }

                return left_comparate_order._timestamp==right_comparate_order._timestamp&&
                       left_comparate_order._position<right_comparate_order._position;
            }
        };

        std::unordered_map<std::string,
                           const order* const> _order_dictionary;
        std::set<order,
                 order_comparator> _buy_order_book;
        std::set<order,
                 order_comparator> _sell_order_book;

        order::order_type byte_order_type_convert(const std::uint8_t order_type)
        {
            switch(order_type)
            {
                case static_cast<std::uint8_t>('E'):
                {
                    return order::order_type::EXACT;
                }

                case static_cast<std::uint8_t>('I'):
                {
                    return order::order_type::INSERT_OR_CANCEL;
                }

                case static_cast<std::uint8_t>('L'):
                {
                    return order::order_type::LIMIT;
                }

                case static_cast<std::uint8_t>('M'):
                {
                    return order::order_type::MARKET;
                }

                case static_cast<std::uint8_t>('U'):
                {
                    return order::order_type::EXACT_INSERT_OR_CANCEL;
                }

                default:
                {
                    return order::order_type::INVALID;
                }
            }
        }

        order::side byte_side_convert(const std::uint8_t side)
        {
            switch(side)
            {
                case static_cast<std::uint8_t>('B'):
                {
                    return order::side::BUY;
                }

                case static_cast<std::uint8_t>('S'):
                {
                    return order::side::SELL;
                }

                default:
                {
                    return order::side::INVALID;
                }
            }
        }

        bool insert_order(const order& insert_order)
        {
            const std::unordered_map<std::string,
                                     const order* const>::const_iterator order_dictionary_entry(_order_dictionary.find(insert_order._order_id));
            if(order_dictionary_entry!=_order_dictionary.cend())
            {
                return false;
            }

            else if((_configuration._timestamp_configuration!=configuration::timestamp_configuration::IGNORE)&&
                    (insert_order._timestamp==common_types::INVALID_TIMESTAMP||
                     insert_order._timestamp<_last_timestamp))
            {
                return false;
            }

            const order* order_handle(nullptr);
            if(insert_order._side==order::side::BUY)
            {
                order_handle=&(*_buy_order_book.emplace(insert_order).first);
            }

            else
            {
                order_handle=&(*_sell_order_book.emplace(insert_order).first);
            }

            _order_dictionary.emplace(insert_order._order_id,
                                      order_handle);

            return true;
        }

        bool erase_order(const std::string& order_id,
                         const std::uintmax_t timestamp=common_types::INVALID_TIMESTAMP,
                         order* const order_handle=nullptr)
        {
            const std::unordered_map<std::string,
                                     const order* const>::const_iterator order_dictionary_entry(_order_dictionary.find(order_id));
            if((order_dictionary_entry==_order_dictionary.cend())||
               (_configuration._timestamp_configuration!=configuration::timestamp_configuration::IGNORE&&
                timestamp!=common_types::INVALID_TIMESTAMP&&
                order_dictionary_entry->second->_timestamp!=timestamp))
            {
                return false;
            }

            const order remove_order(*order_dictionary_entry->second);
            if(order_handle)
            {
                *order_handle=remove_order;
            }

            _order_dictionary.erase(order_dictionary_entry);
            if(remove_order._side==order::side::BUY)
            {
                _buy_order_book.erase(remove_order);
            }

            else
            {
                _sell_order_book.erase(remove_order);
            }

            return true;
        }

        std::uintmax_t erase_orders(const std::uintmax_t timestamp,
                                    std::vector<order>* const orders_handle=nullptr)
        {
            std::uintmax_t erase_order_count(static_cast<std::uintmax_t>(0));
            if(timestamp==common_types::INVALID_TIMESTAMP)
            {
                return erase_order_count;
            }

            for(std::unordered_map<std::string,
                                   const order* const>::const_iterator order_dictionary_entry(_order_dictionary.cbegin());
                order_dictionary_entry!=_order_dictionary.cend();)
            {
                const order remove_order(*order_dictionary_entry->second);
                if(remove_order._timestamp<=timestamp)
                {
                    if(orders_handle)
                    {
                        orders_handle->emplace_back(remove_order);
                    }

                    order_dictionary_entry=_order_dictionary.erase(order_dictionary_entry);
                    if(remove_order._side==order::side::BUY)
                    {
                        _buy_order_book.erase(remove_order);
                    }

                    else
                    {
                        _sell_order_book.erase(remove_order);
                    }

                    ++erase_order_count;
                }

                else
                {
                    ++order_dictionary_entry;
                }
            }

            return erase_order_count;
        }

        std::uintmax_t find_orders(const std::string& symbol,
                                   const std::uintmax_t timestamp=common_types::INVALID_TIMESTAMP,
                                   std::vector<const order*>* const orders_handle=nullptr)
        {
            std::uintmax_t find_order_count(static_cast<std::uintmax_t>(0));
            for(std::unordered_map<std::string,
                                   const order* const>::const_iterator order_dictionary_entry(_order_dictionary.cbegin());
                order_dictionary_entry!=_order_dictionary.cend();
                ++order_dictionary_entry)
            {
                const order* const find_order(order_dictionary_entry->second);
                if(find_order->_symbol!=symbol)
                {
                    continue;
                }

                else if(_configuration._timestamp_configuration!=configuration::timestamp_configuration::IGNORE&&
                        timestamp!=common_types::INVALID_TIMESTAMP&&
                        find_order->_timestamp!=timestamp)
                {
                    continue;
                }

                else if(orders_handle)
                {
                    orders_handle->emplace_back(find_order);
                }

                ++find_order_count;
            }

            return find_order_count;
        }

        std::uintmax_t find_orders(const std::uintmax_t timestamp,
                                   std::vector<const order*>* const orders_handle=nullptr)
        {
            std::uintmax_t find_order_count(static_cast<std::uintmax_t>(0));
            if(timestamp==common_types::INVALID_TIMESTAMP)
            {
                return find_order_count;
            }

            for(std::unordered_map<std::string,
                                   const order* const>::const_iterator order_dictionary_entry(_order_dictionary.cbegin());
                order_dictionary_entry!=_order_dictionary.cend();
                ++order_dictionary_entry)
            {
                const order* const find_order(order_dictionary_entry->second);
                if(find_order->_timestamp<=timestamp)
                {
                    if(orders_handle)
                    {
                        orders_handle->emplace_back(find_order);
                    }

                    ++find_order_count;
                }
            }

            return find_order_count;
        }

        std::vector<order> move_orders(const order::side side,
                                       const std::string& symbol="",
                                       const std::uintmax_t timestamp=common_types::INVALID_TIMESTAMP)
        {
            std::vector<order> orders;
            std::set<order,
                     order_comparator>& order_book(side==order::side::BUY?
                                                   _buy_order_book:
                                                   _sell_order_book);
            for(std::set<order,
                         order_comparator>::const_iterator order_book_entry(order_book.cbegin());
                order_book_entry!=order_book.cend();)
            {
                if(!symbol.empty()&&
                   order_book_entry->_symbol!=symbol)
                {
                    ++order_book_entry;
                }

                else if(_configuration._timestamp_configuration!=configuration::timestamp_configuration::IGNORE&&
                        timestamp!=common_types::INVALID_TIMESTAMP&&
                        order_book_entry->_timestamp>timestamp)
                {
                    ++order_book_entry;
                }

                else
                {
                    orders.emplace_back(*order_book_entry);
                    _order_dictionary.erase(order_book_entry->_order_id);
                    order_book_entry=order_book.erase(order_book_entry);
                }
            }

            return orders;
        }

        void reinsert_orders(const order::side side,
                             std::vector<order>&& orders)
        {
            std::set<order,
                     order_comparator>& order_book(side==order::side::BUY?
                                                   _buy_order_book:
                                                   _sell_order_book);
            for(order& reinsert_order:orders)
            {
                if((reinsert_order._match_status==order::match_status::NO_MATCH||
                    reinsert_order._match_status==order::match_status::PARTIAL_MATCH)&&
                   (reinsert_order._order_type!=order::order_type::EXACT_INSERT_OR_CANCEL&&
                    reinsert_order._order_type!=order::order_type::INSERT_OR_CANCEL))
                {
                    const order* const order_handle(&(*order_book.emplace(std::move(reinsert_order)).first));
                    _order_dictionary.emplace(order_handle->_order_id,
                                              order_handle);
                }
            }
        }

        bool process_amend_instruction(const instruction& amend_instruction)
        {
            logger_stream()<<"Performing AMEND operation...\n";

            order amend_order;
            if(!erase_order(amend_instruction._order_id,
                            common_types::INVALID_TIMESTAMP,
                            &amend_order))
            {
                return log_error(ORDER_NOT_FOUND_ERROR);
            }

            const order previous_amend_order(amend_order);

            if(amend_instruction._order_type!=common_types::INVALID_ORDER_TYPE)
            {
                amend_order._order_type=byte_order_type_convert(amend_instruction._order_type);
            }

            if(amend_instruction._side!=common_types::INVALID_SIDE)
            {
                amend_order._side=byte_side_convert(amend_instruction._side);
            }

            if(amend_instruction._price!=common_types::INVALID_PRICE)
            {
                amend_order._price=amend_instruction._price;
            }

            if(amend_instruction._quantity!=common_types::INVALID_QUANTITY)
            {
                if(amend_order._match_status!=order::match_status::NO_MATCH)
                {
                    insert_order(previous_amend_order);

                    return log_error("match_engine_error: Invalid amend quantity");
                }

                amend_order._quantity=amend_instruction._quantity;
            }

            if(_configuration._timestamp_configuration!=configuration::timestamp_configuration::IGNORE)
            {
                amend_order._timestamp=amend_instruction._timestamp;
            }

            if(_configuration._strict_amend)
            {
                amend_order._position=(_last_position++);
            }

            const configuration::timestamp_configuration store_timestamp_configuration(_configuration._timestamp_configuration);
            _configuration._timestamp_configuration=configuration::timestamp_configuration::IGNORE;

            insert_order(amend_order);

            _configuration._timestamp_configuration=store_timestamp_configuration;

            logger_stream()<<"Order "
                           <<previous_amend_order<<" -> "
                           <<amend_order
                           <<" amend SUCCESS\n";

            return true;
        }

        bool process_cancel_instruction(const instruction& cancel_instruction)
        {
            logger_stream()<<"Performing CANCEL operation...\n";

            if(!cancel_instruction._order_id.empty())
            {
                order cancel_order;
                if(!erase_order(cancel_instruction._order_id,
                                cancel_instruction._timestamp,
                                &cancel_order))
                {
                    return log_error(ORDER_NOT_FOUND_ERROR);
                }

                logger_stream()<<"Order "
                               <<cancel_order
                               <<" cancel SUCCESS\n";

                return true;
            }

            else if(cancel_instruction._timestamp!=common_types::INVALID_TIMESTAMP)
            {
                std::vector<order> cancel_orders;
                if(!erase_orders(cancel_instruction._timestamp,
                                 &cancel_orders))
                {
                    logger_stream()<<"Orders cancel SUCCESS\n";
                }

                else
                {
                    for(const order& cancel_order:cancel_orders)
                    {
                        logger_stream()<<"Order "
                                       <<cancel_order
                                       <<" cancel SUCCESS\n";
                    }
                }

                return true;
            }

            _order_dictionary.clear();
            _buy_order_book.clear();
            _sell_order_book.clear();

            logger_stream()<<"Orders cancel SUCCESS\n";

            return true;
        }

        bool process_match_instruction(const instruction& match_instruction)
        {
            logger_stream()<<"Performing MATCH operation...\n";

            std::vector<order> buy_orders(move_orders(order::side::BUY,
                                                      match_instruction._symbol,
                                                      match_instruction._timestamp));
            std::vector<order> sell_orders(move_orders(order::side::SELL,
                                                       match_instruction._symbol,
                                                       match_instruction._timestamp));

            order_matcher::match_orders(buy_orders,
                                        sell_orders,
                                        logger_stream());

            reinsert_orders(order::side::BUY,
                            std::move(buy_orders));
            reinsert_orders(order::side::SELL,
                            std::move(sell_orders));

            logger_stream()<<"Orders match SUCCESS\n";

            return true;
        }

        bool process_order_instruction(const instruction& order_instruction)
        {
            logger_stream()<<"Performing ORDER operation...\n";

            order new_order;
            new_order._order_id=order_instruction._order_id;
            new_order._symbol=order_instruction._symbol;
            new_order._order_type=byte_order_type_convert(order_instruction._order_type);
            new_order._side=byte_side_convert(order_instruction._side);
            new_order._price=order_instruction._price;
            new_order._quantity=order_instruction._quantity;

            if(_configuration._timestamp_configuration!=configuration::timestamp_configuration::IGNORE&&
               order_instruction._timestamp!=common_types::INVALID_TIMESTAMP)
            {
                new_order._timestamp=order_instruction._timestamp;
                _last_timestamp=std::max(_last_timestamp,
                                         new_order._timestamp);
            }

            new_order._match_status=order::match_status::NO_MATCH;
            if(_configuration._insert_match)
            {
                std::vector<order> orders(move_orders(new_order._side==order::side::BUY?
                                                      order::side::SELL:
                                                      order::side::BUY));
                order_matcher::match_order(new_order,
                                           orders,
                                           logger_stream());
                reinsert_orders(new_order._side==order::side::BUY?
                                order::side::SELL:
                                order::side::BUY,
                                std::move(orders));
                if(new_order._match_status==order::match_status::FULL_MATCH)
                {
                    logger_stream()<<"Order "
                                   <<new_order
                                   <<" insert match SUCCESS\n";

                    return true;
                }

                else if(new_order._match_status==order::match_status::INVALID)
                {
                    return true;
                }
            }

            new_order._position=(_last_position++);
            if(!insert_order(new_order))
            {
                std::ostringstream new_order_stream;
                new_order_stream<<"match_engine_error: Order "
                                <<new_order
                                <<" insert failure";

                return log_error(new_order_stream.str());
            }

            logger_stream()<<"Order "
                           <<new_order
                           <<" insert SUCCESS\n";

            return true;
        }

        bool process_query_instruction(const instruction& query_instruction)
        {
            logger_stream()<<"Performing QUERY operation...\n";

            std::vector<const order*> orders;
            if(!query_instruction._symbol.empty())
            {
                find_orders(query_instruction._symbol,
                            query_instruction._timestamp,
                            &orders);
            }

            else if(query_instruction._timestamp!=common_types::INVALID_TIMESTAMP)
            {
                find_orders(query_instruction._timestamp,
                            &orders);
            }

            else
            {
                for(const std::unordered_map<std::string,
                                             const order* const>::value_type& order_dictionary_entry:_order_dictionary)
                {
                    orders.emplace_back(order_dictionary_entry.second);
                }
            }

            if(orders.empty())
            {
                logger_stream()<<"Orders query SUCCESS\n";

                return true;
            }

            for(const order* order:orders)
            {
                logger_stream()<<"Order "
                               <<*order
                               <<" query SUCCESS\n";
            }

            return true;
        }

        bool process_instruction(const instruction& match_engine_instruction)
        {
            switch(match_engine_instruction._instruction_type)
            {
                case instruction::instruction_type::AMEND:
                {
                    return process_amend_instruction(match_engine_instruction);
                }

                case instruction::instruction_type::CANCEL:
                {
                    return process_cancel_instruction(match_engine_instruction);
                }

                case instruction::instruction_type::MATCH:
                {
                    return process_match_instruction(match_engine_instruction);
                }

                case instruction::instruction_type::ORDER:
                {
                    return process_order_instruction(match_engine_instruction);
                }

                case instruction::instruction_type::QUERY:
                {
                    return process_query_instruction(match_engine_instruction);
                }

                default:
                {
                    return log_error(match_engine_instruction._error);
                }
            }
        }
    }

    bool configure(const std::vector<std::string>& command_line)
    {
        _configuration=command_line_parser::parse_command_line(command_line);

        if(!_configuration._output_file_path.empty())
        {
            _output_file_stream.open(_configuration._output_file_path);
            if(_output_file_stream.is_open())
            {
                _logger_stream=_output_file_stream;
            }

            else if(!log_error("match_engine_error: Output file path open failure"))
            {
                return false;
            }
        }

        if(!_configuration._errors.empty())
        {
            for(const std::string& configuration_error:_configuration._errors)
            {
                log_error(configuration_error);
            }

            if(!_configuration._process_errors)
            {
                return false;
            }
        }

        _instruction_file_stream.open(_configuration._instruction_file_path);
        if(!_instruction_file_stream.is_open())
        {
            return log_error("match_engine_error: Instruction file path open failure");
        }

        return true;
    }

    bool process()
    {
        std::string instruction_line;
        while(std::getline(_instruction_file_stream,
                           instruction_line))
        {
            if(instruction_line.empty()||
               instruction_line.front()=='#')
            {
                continue;
            }

            logger_stream()<<"Processing instruction {"
                           <<instruction_line
                           <<"}...\n";

            const instruction match_engine_instruction(instruction_parser::parse_instruction(instruction_line));
            if(!process_instruction(match_engine_instruction))
            {
                return false;
            }
        }

        return true;
    }
}
