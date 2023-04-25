#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#include<cstdint>
#include<string>
#include<vector>

struct configuration
{
    enum class timestamp_configuration:std::uint8_t
    {
        IGNORE,
        RELAX,
        STRICT
    };

    std::vector<std::string> _errors;
    std::string _instruction_file_path;
    std::string _output_file_path;
    timestamp_configuration _timestamp_configuration;
    bool _ignore_errors;
    bool _insert_match;
    bool _process_errors;
    bool _strict_amend;

    configuration();
};

#endif
