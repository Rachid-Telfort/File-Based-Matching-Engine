#ifndef INSTRUCTION_PARSER_H_INCLUDED
#define INSTRUCTION_PARSER_H_INCLUDED

#include<cstdint>
#include<string>
#include"instruction.h"

namespace instruction_parser
{
    instruction parse_instruction(const std::string& instruction_line);
}

#endif
