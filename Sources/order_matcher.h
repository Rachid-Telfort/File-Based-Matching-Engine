#ifndef ORDER_MATCHER_H_INCLUDED
#define ORDER_MATCHER_H_INCLUDED

#include<ostream>
#include<vector>
#include"order.h"

namespace order_matcher
{
    void match_order(order&,
                     std::vector<order>&,
                     std::ostream&);
    void match_orders(std::vector<order>& buy_orders,
                      std::vector<order>& sell_orders,
                      std::ostream& logger_stream);
}

#endif
