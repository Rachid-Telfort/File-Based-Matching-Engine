#include<string>
#include"common_types.h"
#include"order_matcher.h"

namespace order_matcher
{
    namespace
    {
        bool matchable_order(const order& order,
                             const std::string& symbol)
        {
            return (order._match_status==order::match_status::NO_MATCH||
                    order._match_status==order::match_status::PARTIAL_MATCH)&&
                   (order._symbol==symbol);
        }

        order* next_exact_order(std::vector<order>& orders,
                                const std::string& symbol,
                                const long double order_price)
        {
            for(order& order:orders)
            {
                if(matchable_order(order,
                                   symbol)&&
                   common_types::decimal_equals(order._price,
                                                order_price))
                {
                    return &order;
                }
            }

            return nullptr;
        }

        order* next_limit_order(std::vector<order>& orders,
                                const std::string& symbol,
                                const long double order_price,
                                const order::side side)
        {
            for(order& order:orders)
            {
                if(!matchable_order(order,
                                    symbol))
                {
                    continue;
                }

                else if(side==order::side::BUY&&
                        order._price>=order_price)
                {
                    return &order;
                }

                else if(side==order::side::SELL&&
                        order._price<=order_price)
                {
                    return &order;
                }
            }

            return nullptr;
        }

        order* next_market_order(std::vector<order>& orders,
                                 const std::string& symbol)
        {
            for(order& order:orders)
            {
                if(matchable_order(order,
                                   symbol))
                {
                    return &order;
                }
            }

            return nullptr;
        }

        void match_orders(order& buy_order,
                          order& sell_order,
                          std::ostream& logger_stream)
        {
            if(buy_order._quantity<=sell_order._quantity)
            {
                sell_order._quantity-=buy_order._quantity;
                if(common_types::decimal_equals(sell_order._quantity,
                                                common_types::ZERO_DECIMAL))
                {
                    sell_order._quantity=common_types::ZERO_DECIMAL;
                    sell_order._match_status=order::match_status::FULL_MATCH;
                }

                else
                {
                    sell_order._match_status=order::match_status::PARTIAL_MATCH;
                }

                buy_order._quantity=common_types::ZERO_DECIMAL;
                buy_order._match_status=order::match_status::FULL_MATCH;
            }

            else
            {
                buy_order._quantity-=sell_order._quantity;
                if(common_types::decimal_equals(buy_order._quantity,
                                                common_types::ZERO_DECIMAL))
                {
                    buy_order._quantity=common_types::ZERO_DECIMAL;
                    buy_order._match_status=order::match_status::FULL_MATCH;
                }

                else
                {
                    buy_order._match_status=order::match_status::PARTIAL_MATCH;
                }

                sell_order._quantity=common_types::ZERO_DECIMAL;
                sell_order._match_status=order::match_status::FULL_MATCH;
            }

            logger_stream<<"MATCH: "
                         <<buy_order
                         <<" <-> "
                         <<sell_order
                         <<'\n';
        }
    }

    void match_order(order& match_order,
                     std::vector<order>& orders,
                     std::ostream& logger_stream)
    {
        if(orders.empty())
        {
            return;
        }

        while(match_order._match_status!=order::match_status::FULL_MATCH&&
              match_order._match_status!=order::match_status::INVALID)
        {
            if(match_order._order_type==order::order_type::EXACT||
               match_order._order_type==order::order_type::EXACT_INSERT_OR_CANCEL)
            {
                order* exact_order(next_exact_order(orders,
                                                    match_order._symbol,
                                                    match_order._price));
                if(exact_order)
                {
                    match_orders(match_order,
                                 *exact_order,
                                 logger_stream);
                }

                else if(match_order._order_type==order::order_type::EXACT_INSERT_OR_CANCEL)
                {
                    match_order._match_status=order::match_status::INVALID;
                }

                else
                {
                    break;
                }
            }

            else if(match_order._order_type==order::order_type::INSERT_OR_CANCEL||
                    match_order._order_type==order::order_type::LIMIT)
            {
                order* limit_order(next_limit_order(orders,
                                                    match_order._symbol,
                                                    match_order._price,
                                                    match_order._side==order::side::BUY?
                                                    order::side::SELL:
                                                    order::side::BUY));
                if(limit_order)
                {
                    match_orders(match_order,
                                 *limit_order,
                                 logger_stream);
                }

                else if(match_order._order_type==order::order_type::INSERT_OR_CANCEL)
                {
                    match_order._match_status=order::match_status::INVALID;
                }

                else
                {
                    break;
                }
            }

            else
            {
                order* market_order(next_market_order(orders,
                                                      match_order._symbol));
                if(market_order)
                {
                    match_orders(match_order,
                                 *market_order,
                                 logger_stream);
                }

                else
                {
                    break;
                }
            }
        }
    }

    void match_orders(std::vector<order>& buy_orders,
                      std::vector<order>& sell_orders,
                      std::ostream& logger_stream)
    {
        for(order& buy_order:buy_orders)
        {
            match_order(buy_order,
                        sell_orders,
                        logger_stream);
        }

        for(order& sell_order:sell_orders)
        {
            match_order(sell_order,
                        buy_orders,
                        logger_stream);
        }
    }
}
