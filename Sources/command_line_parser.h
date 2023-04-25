#ifndef COMMAND_LINE_PARSER_H_INCLUDED
#define COMMAND_LINE_PARSER_H_INCLUDED

#include<string>
#include<vector>
#include"configuration.h"

namespace command_line_parser
{
    configuration parse_command_line(const std::vector<std::string>& command_line);
}

#endif
