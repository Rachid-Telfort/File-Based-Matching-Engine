#include<sstream>
#include<stdexcept>
#include<utility>
#include"common_types.h"
#include"instruction_parser.h"

namespace instruction_parser
{
    namespace
    {
        const std::string INVALID_ORDER_ID_ERROR("instruction_parser_error: Invalid order ID");
        const std::string INVALID_ORDER_TYPE_ERROR("instruction_parser_error: Invalid order type");
        const std::string INVALID_PRICE_ERROR("instruction_parser_error: Invalid price");
        const std::string INVALID_QUANTITY_ERROR("instruction_parser_error: Invalid quantity");
        const std::string INVALID_SIDE_ERROR("instruction_parser_error: Invalid side");
        const std::string INVALID_SYMBOL_ERROR("instruction_parser_error: Invalid symbol");

        std::uint8_t parse_order_type(const std::string& order_type_format)
        {
            if(order_type_format.empty()||
               order_type_format.size()!=static_cast<std::string::size_type>(1))
            {
                return common_types::INVALID_ORDER_TYPE;
            }

            if(order_type_format.front()=='E'||
               order_type_format.front()=='I'||
               order_type_format.front()=='L'||
               order_type_format.front()=='M'||
               order_type_format.front()=='U')
            {
                return static_cast<std::uint8_t>(order_type_format.front());
            }

            return common_types::INVALID_ORDER_TYPE;
        }

        std::uint8_t parse_side(const std::string& side_format)
        {
            if(side_format.empty()||
               side_format.size()!=static_cast<std::string::size_type>(1))
            {
                return common_types::INVALID_SIDE;
            }

            if(side_format.front()=='B'||
               side_format.front()=='S')
            {
                return static_cast<std::uint8_t>(side_format.front());
            }

            return common_types::INVALID_SIDE;
        }

        long double parse_decimal(const std::string& decimal_format,
                                  const long double invalid_decimal)
        {
            long double decimal(common_types::ZERO_DECIMAL);

            try
            {
                decimal=std::stold(decimal_format);
            }

            catch(const std::invalid_argument& invalid_argument)
            {
                return invalid_decimal;
            }

            catch(const std::out_of_range& out_of_range)
            {
                return invalid_decimal;
            }

            if(decimal<=common_types::ZERO_DECIMAL)
            {
                return invalid_decimal;
            }

            return decimal;
        }

        std::uintmax_t parse_timestamp(const std::string& timestamp_format)
        {
            std::uintmax_t timestamp(static_cast<std::uintmax_t>(0));

            try
            {
                timestamp=static_cast<std::uintmax_t>(std::stoull(timestamp_format));
            }

            catch(const std::invalid_argument& invalid_argument)
            {
                return common_types::INVALID_TIMESTAMP;
            }

            catch(const std::out_of_range& out_of_range)
            {
                return common_types::INVALID_TIMESTAMP;
            }

            if(timestamp>static_cast<std::uintmax_t>(86399))
            {
                return common_types::INVALID_TIMESTAMP;
            }

            return timestamp;
        }

        instruction invalid_instruction_with_error(const std::string& error)
        {
            instruction invalid_instruction;
            invalid_instruction._error=error;

            return invalid_instruction;
        }

        std::string next_component(std::istringstream& component_format_stream)
        {
            std::string component_format;
            std::getline(component_format_stream,
                         component_format,
                         '/');

            return component_format;
        }

        instruction parse_amend_instruction(std::istringstream& amend_instruction_format_stream)
        {
            instruction amend_instruction;
            amend_instruction._instruction_type=instruction::instruction_type::AMEND;

            std::string amend_instruction_format(next_component(amend_instruction_format_stream));
            if(amend_instruction_format.empty())
            {
                return invalid_instruction_with_error(INVALID_ORDER_ID_ERROR);
            }

            amend_instruction._order_id=std::move(amend_instruction_format);

            amend_instruction_format=next_component(amend_instruction_format_stream);
            const std::uint8_t order_type(parse_order_type(amend_instruction_format));
            if(!amend_instruction_format.empty()&&
               order_type==common_types::INVALID_ORDER_TYPE)
            {
                return invalid_instruction_with_error(INVALID_ORDER_TYPE_ERROR);
            }

            amend_instruction._order_type=order_type;

            amend_instruction_format=next_component(amend_instruction_format_stream);
            const std::uint8_t side(parse_side(amend_instruction_format));
            if(!amend_instruction_format.empty()&&
               side==common_types::INVALID_SIDE)
            {
                return invalid_instruction_with_error(INVALID_SIDE_ERROR);
            }

            amend_instruction._side=side;

            amend_instruction_format=next_component(amend_instruction_format_stream);
            const long double price(parse_decimal(amend_instruction_format,
                                                  common_types::INVALID_PRICE));
            if(!amend_instruction_format.empty()&&
               price==common_types::INVALID_PRICE)
            {
                return invalid_instruction_with_error(INVALID_PRICE_ERROR);
            }

            amend_instruction._price=price;

            amend_instruction_format=next_component(amend_instruction_format_stream);
            const long double quantity(parse_decimal(amend_instruction_format,
                                                     common_types::INVALID_QUANTITY));
            if(!amend_instruction_format.empty()&&
               quantity==common_types::INVALID_QUANTITY)
            {
                return invalid_instruction_with_error(INVALID_QUANTITY_ERROR);
            }

            amend_instruction._quantity=quantity;

            amend_instruction_format=next_component(amend_instruction_format_stream);
            const std::uintmax_t timestamp(parse_timestamp(amend_instruction_format));
            amend_instruction._timestamp=timestamp;

            if(amend_instruction._order_type==common_types::INVALID_ORDER_TYPE&&
               amend_instruction._side==common_types::INVALID_SIDE&&
               amend_instruction._price==common_types::INVALID_PRICE&&
               amend_instruction._quantity==common_types::INVALID_QUANTITY&&
               amend_instruction._timestamp==common_types::INVALID_TIMESTAMP)
            {
                return invalid_instruction_with_error("instruction_parser_error: Invalid amend instruction");
            }

            return amend_instruction;
        }

        instruction parse_cancel_instruction(std::istringstream& cancel_instruction_format_stream)
        {
            instruction cancel_instruction;
            cancel_instruction._instruction_type=instruction::instruction_type::CANCEL;

            std::string cancel_instruction_format(next_component(cancel_instruction_format_stream));
            cancel_instruction._order_id=std::move(cancel_instruction_format);

            cancel_instruction_format=next_component(cancel_instruction_format_stream);
            const std::uintmax_t timestamp(parse_timestamp(cancel_instruction_format));
            cancel_instruction._timestamp=timestamp;

            return cancel_instruction;
        }

        instruction parse_match_instruction(std::istringstream& match_instruction_format_stream)
        {
            instruction match_instruction;
            match_instruction._instruction_type=instruction::instruction_type::MATCH;

            std::string match_instruction_format(next_component(match_instruction_format_stream));
            match_instruction._symbol=std::move(match_instruction_format);

            match_instruction_format=next_component(match_instruction_format_stream);
            const std::uintmax_t timestamp(parse_timestamp(match_instruction_format));
            match_instruction._timestamp=timestamp;

            return match_instruction;
        }

        instruction parse_order_instruction(std::istringstream& order_instruction_format_stream)
        {
            instruction order_instruction;
            order_instruction._instruction_type=instruction::instruction_type::ORDER;

            std::string order_instruction_format(next_component(order_instruction_format_stream));
            if(order_instruction_format.empty())
            {
                return invalid_instruction_with_error(INVALID_ORDER_ID_ERROR);
            }

            order_instruction._order_id=std::move(order_instruction_format);

            order_instruction_format=next_component(order_instruction_format_stream);
            if(order_instruction_format.empty())
            {
                return invalid_instruction_with_error(INVALID_SYMBOL_ERROR);
            }

            order_instruction._symbol=std::move(order_instruction_format);

            order_instruction_format=next_component(order_instruction_format_stream);
            const std::uint8_t order_type(parse_order_type(order_instruction_format));
            if(order_type==common_types::INVALID_ORDER_TYPE)
            {
                return invalid_instruction_with_error(INVALID_ORDER_TYPE_ERROR);
            }

            order_instruction._order_type=order_type;

            order_instruction_format=next_component(order_instruction_format_stream);
            const std::uint8_t side(parse_side(order_instruction_format));
            if(side==common_types::INVALID_SIDE)
            {
                return invalid_instruction_with_error(INVALID_SIDE_ERROR);
            }

            order_instruction._side=side;

            order_instruction_format=next_component(order_instruction_format_stream);
            const long double price(parse_decimal(order_instruction_format,
                                                  common_types::INVALID_PRICE));
            if(price==common_types::INVALID_PRICE)
            {
                return invalid_instruction_with_error(INVALID_PRICE_ERROR);
            }

            order_instruction._price=price;

            order_instruction_format=next_component(order_instruction_format_stream);
            const long double quantity(parse_decimal(order_instruction_format,
                                                     common_types::INVALID_QUANTITY));
            if(quantity==common_types::INVALID_QUANTITY)
            {
                return invalid_instruction_with_error(INVALID_QUANTITY_ERROR);
            }

            order_instruction._quantity=quantity;

            order_instruction_format=next_component(order_instruction_format_stream);
            const std::uintmax_t timestamp(parse_timestamp(order_instruction_format));
            order_instruction._timestamp=timestamp;

            return order_instruction;
        }

        instruction parse_query_instruction(std::istringstream& query_instruction_format_stream)
        {
            instruction query_instruction;
            query_instruction._instruction_type=instruction::instruction_type::QUERY;

            std::string query_instruction_format(next_component(query_instruction_format_stream));
            query_instruction._symbol=std::move(query_instruction_format);

            query_instruction_format=next_component(query_instruction_format_stream);
            const std::uintmax_t timestamp(parse_timestamp(query_instruction_format));
            query_instruction._timestamp=timestamp;

            return query_instruction;
        }
    }

    instruction parse_instruction(const std::string& instruction_line)
    {
        std::istringstream instruction_format_stream(instruction_line);
        const std::string instruction_format(next_component(instruction_format_stream));
        if(instruction_format.empty()||
           instruction_format.size()!=static_cast<std::string::size_type>(1))
        {
            return invalid_instruction_with_error("instruction_parser_error: Invalid instruction code");
        }

        switch(instruction_format.front())
        {
            case 'A':
            {
                return parse_amend_instruction(instruction_format_stream);
            }

            case 'C':
            {
                return parse_cancel_instruction(instruction_format_stream);
            }

            case 'M':
            {
                return parse_match_instruction(instruction_format_stream);
            }

            case 'O':
            {
                return parse_order_instruction(instruction_format_stream);
            }

            case 'Q':
            {
                return parse_query_instruction(instruction_format_stream);
            }

            default:
            {
                return invalid_instruction_with_error("instruction_parser_error: Invalid instruction");
            }
        }
    }
}
