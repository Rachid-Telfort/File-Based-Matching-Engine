#include"common_types.h"
#include"instruction.h"

instruction::instruction():_error(),
                           _order_id(),
                           _symbol(),
                           _price(common_types::INVALID_PRICE),
                           _quantity(common_types::INVALID_QUANTITY),
                           _timestamp(common_types::INVALID_TIMESTAMP),
                           _order_type(common_types::INVALID_ORDER_TYPE),
                           _side(common_types::INVALID_SIDE),
                           _instruction_type(instruction_type::INVALID){}
