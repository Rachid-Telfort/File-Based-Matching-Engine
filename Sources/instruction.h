#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED

#include<cstdint>
#include<string>

struct instruction
{
    enum class instruction_type:std::uint8_t
    {
        AMEND,
        CANCEL,
        INVALID,
        MATCH,
        ORDER,
        QUERY
    };

    std::string _error;
    std::string _order_id;
    std::string _symbol;
    long double _price;
    long double _quantity;
    std::uintmax_t _timestamp;
    std::uint8_t _order_type;
    std::uint8_t _side;
    instruction_type _instruction_type;

    instruction();
};

#endif
