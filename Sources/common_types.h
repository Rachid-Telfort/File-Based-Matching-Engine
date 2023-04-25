#ifndef COMMON_TYPES_H_INCLUDED
#define COMMON_TYPES_H_INCLUDED

#include<cstdint>

namespace common_types
{
    const extern std::uint8_t INVALID_ORDER_TYPE;
    const extern std::uint8_t INVALID_SIDE;
    const extern std::uintmax_t INVALID_TIMESTAMP;
    const extern std::uintmax_t INVALID_POSITION;
    const extern long double INVALID_PRICE;
    const extern long double INVALID_QUANTITY;
    const extern long double ZERO_DECIMAL;

    bool decimal_equals(const long double left_comparate_decimal,
                        const long double right_comparate_decimal);
}

#endif
