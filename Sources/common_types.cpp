#include<cmath>
#include"common_types.h"

namespace common_types
{
    const std::uint8_t INVALID_ORDER_TYPE(static_cast<std::uint8_t>('\0'));
    const std::uint8_t INVALID_SIDE(static_cast<std::uint8_t>('\0'));
    const std::uintmax_t INVALID_TIMESTAMP(static_cast<std::uintmax_t>(-1));
    const std::uintmax_t INVALID_POSITION(static_cast<std::uintmax_t>(-1));
    const long double INVALID_PRICE(static_cast<long double>(-1));
    const long double INVALID_QUANTITY(static_cast<long double>(-1));
    const long double ZERO_DECIMAL(static_cast<long double>(0));

    bool decimal_equals(const long double left_comparate_decimal,
                        const long double right_comparate_decimal)
    {
        return std::fabs(left_comparate_decimal-
                         right_comparate_decimal)<=static_cast<long double>(0.001);
    }
}
