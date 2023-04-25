#include<iomanip>
#include<sstream>
#include"common_types.h"
#include"order.h"

namespace
{
    std::uint8_t order_type_byte_convert(const order::order_type order_type)
    {
        switch(order_type)
        {
            case order::order_type::EXACT:
            {
                return static_cast<std::uint8_t>('E');
            }

            case order::order_type::EXACT_INSERT_OR_CANCEL:
            {
                return static_cast<std::uint8_t>('U');
            }

            case order::order_type::INSERT_OR_CANCEL:
            {
                return static_cast<std::uint8_t>('I');
            }

            case order::order_type::LIMIT:
            {
                return static_cast<std::uint8_t>('L');
            }

            case order::order_type::MARKET:
            {
                return static_cast<std::uint8_t>('M');
            }

            default:
            {
                return common_types::INVALID_ORDER_TYPE;
            }
        }
    }

    std::uint8_t side_byte_convert(const order::side side)
    {
        switch(side)
        {
            case order::side::BUY:
            {
                return static_cast<std::uint8_t>('B');
            }

            case order::side::SELL:
            {
                return static_cast<std::uint8_t>('S');
            }

            default:
            {
                return common_types::INVALID_SIDE;
            }
        }
    }

    std::string match_status_string_convert(const order::match_status match_status)
    {
        switch(match_status)
        {
            case order::match_status::FULL_MATCH:
            {
                return "FULL_MATCH";
            }

            case order::match_status::NO_MATCH:
            {
                return "NO_MATCH";
            }

            case order::match_status::PARTIAL_MATCH:
            {
                return "PARTIAL_MATCH";
            }

            default:
            {
                return "INVALID";
            }
        }
    }

    std::string timestamp_string_convert(std::uintmax_t timestamp)
    {
        if(timestamp==common_types::INVALID_TIMESTAMP)
        {
            return "HH:MM:SS";
        }

        const std::uintmax_t seconds(timestamp%
                                     static_cast<std::uintmax_t>(60));
        timestamp/=static_cast<std::uintmax_t>(60);

        const std::uintmax_t minutes(timestamp%
                                     static_cast<std::uintmax_t>(60));
        timestamp/=static_cast<std::uintmax_t>(60);

        const std::uintmax_t hours(timestamp%
                                   static_cast<std::uintmax_t>(24));

        std::ostringstream timestamp_stream;
        timestamp_stream<<std::setw(2)
                        <<std::setfill('0')
                        <<hours
                        <<':'
                        <<std::setw(2)
                        <<std::setfill('0')
                        <<minutes
                        <<':'
                        <<std::setw(2)
                        <<std::setfill('0')
                        <<seconds;

        return timestamp_stream.str();
    }
}

order::order():_order_id(),
               _symbol(),
               _price(common_types::INVALID_PRICE),
               _quantity(common_types::INVALID_QUANTITY),
               _timestamp(common_types::INVALID_TIMESTAMP),
               _position(common_types::INVALID_POSITION),
               _order_type(order_type::INVALID),
               _side(side::INVALID),
               _match_status(match_status::INVALID){}

std::ostream& operator<<(std::ostream& output_stream,
                         const order& order)
{
    return output_stream<<'<'
                        <<order._order_id
                        <<'|'
                        <<order._symbol
                        <<'|'
                        <<order_type_byte_convert(order._order_type)
                        <<'|'
                        <<side_byte_convert(order._side)
                        <<'|'
                        <<std::fixed
                        <<std::setprecision(2)
                        <<order._price
                        <<'|'
                        <<std::fixed
                        <<std::setprecision(2)
                        <<order._quantity
                        <<'|'
                        <<timestamp_string_convert(order._timestamp)
                        <<'|'
                        <<match_status_string_convert(order._match_status)
                        <<'>';
}
