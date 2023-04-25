#ifndef ORDER_H_INCLUDED
#define ORDER_H_INCLUDED

#include<cstdint>
#include<ostream>
#include<string>

struct order
{
    enum class order_type:std::uint8_t
    {
        EXACT,
        EXACT_INSERT_OR_CANCEL,
        INSERT_OR_CANCEL,
        INVALID,
        LIMIT,
        MARKET
    };

    enum class side:std::uint8_t
    {
        BUY,
        INVALID,
        SELL
    };

    enum class match_status:std::uint8_t
    {
        FULL_MATCH,
        INVALID,
        NO_MATCH,
        PARTIAL_MATCH
    };

    std::string _order_id;
    std::string _symbol;
    long double _price;
    long double _quantity;
    std::uintmax_t _timestamp;
    std::uintmax_t _position;
    order_type _order_type;
    side _side;
    match_status _match_status;

    order();
};

std::ostream& operator<<(std::ostream& output_stream,
                         const order& order);

#endif
