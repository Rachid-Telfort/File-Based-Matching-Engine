#include"command_line_parser.h"

namespace command_line_parser
{
    namespace
    {
        void parse_boolean_option(bool& boolean_option,
                                  std::vector<std::string>& errors,
                                  std::vector<std::string>::size_type& command_line_index,
                                  const std::vector<std::string>& command_line,
                                  const std::string& boolean_option_error)
        {
            if(command_line_index>=command_line.size())
            {
                errors.emplace_back(boolean_option_error);

                return;
            }

            boolean_option=true;
            ++command_line_index;
        }

        void parse_string_option(std::string& string_option,
                                 std::vector<std::string>& errors,
                                 std::vector<std::string>::size_type& command_line_index,
                                 const std::vector<std::string>& command_line,
                                 const std::string& string_option_error)
        {
            ++command_line_index;
            if(command_line_index>=command_line.size())
            {
                errors.emplace_back(string_option_error);

                return;
            }

            string_option=command_line[command_line_index];
            ++command_line_index;
        }

        void parse_timestamp_configuration_option(configuration::timestamp_configuration& timestamp_configuration_option,
                                                  std::vector<std::string>& errors,
                                                  std::vector<std::string>::size_type& command_line_index,
                                                  const std::vector<std::string>& command_line,
                                                  const std::string& timestamp_configuration_option_find_error,
                                                  const std::string& timestamp_configuration_option_valid_error)
        {
            ++command_line_index;
            if(command_line_index>=command_line.size())
            {
                errors.emplace_back(timestamp_configuration_option_find_error);

                return;
            }

            const std::string& command_line_argument(command_line[command_line_index]);
            if(command_line_argument=="IGNORE")
            {
                timestamp_configuration_option=configuration::timestamp_configuration::IGNORE;
            }

            else if(command_line_argument=="RELAX")
            {
                timestamp_configuration_option=configuration::timestamp_configuration::RELAX;
            }

            else if(command_line_argument=="STRICT")
            {
                timestamp_configuration_option=configuration::timestamp_configuration::STRICT;
            }

            else
            {
                errors.emplace_back(timestamp_configuration_option_valid_error);
            }

            ++command_line_index;
        }
    }

    configuration parse_command_line(const std::vector<std::string>& command_line)
    {
        configuration command_line_configuration;

        for(std::vector<std::string>::size_type command_line_index(static_cast<std::vector<std::string>::size_type>(0));
            command_line_index<command_line.size();)
        {
            const std::string& command_line_argument(command_line[command_line_index]);
            if(command_line_argument=="-A")
            {
                parse_boolean_option(command_line_configuration._strict_amend,
                                     command_line_configuration._errors,
                                     command_line_index,
                                     command_line,
                                     "command_line_parser_error: Invalid strict amend option");
            }

            else if(command_line_argument=="-E")
            {
                parse_boolean_option(command_line_configuration._ignore_errors,
                                     command_line_configuration._errors,
                                     command_line_index,
                                     command_line,
                                     "command_line_parser_error: Invalid ignore errors option");
            }

            else if(command_line_argument=="-I")
            {
                parse_string_option(command_line_configuration._instruction_file_path,
                                    command_line_configuration._errors,
                                    command_line_index,
                                    command_line,
                                    "command_line_parser_error: Instruction file path not found");
            }

            else if(command_line_argument=="-M")
            {
                parse_boolean_option(command_line_configuration._insert_match,
                                     command_line_configuration._errors,
                                     command_line_index,
                                     command_line,
                                     "command_line_parser_error: Invalid insert match option");
            }

            else if(command_line_argument=="-O")
            {
                parse_string_option(command_line_configuration._output_file_path,
                                    command_line_configuration._errors,
                                    command_line_index,
                                    command_line,
                                    "command_line_parser_error: Output file path not found");
            }

            else if(command_line_argument=="-P")
            {
                parse_boolean_option(command_line_configuration._process_errors,
                                     command_line_configuration._errors,
                                     command_line_index,
                                     command_line,
                                     "command_line_parser_error: Invalid process errors option");
            }

            else if(command_line_argument=="-T")
            {
                parse_timestamp_configuration_option(command_line_configuration._timestamp_configuration,
                                                     command_line_configuration._errors,
                                                     command_line_index,
                                                     command_line,
                                                     "command_line_parser_error: Timestamp configuration not found",
                                                     "command_line_parser_error: Invalid timestamp configuration option");
            }

            else
            {
                command_line_configuration._errors.emplace_back("command_line_parser_error: Invalid command line option");
                ++command_line_index;
            }
        }

        return command_line_configuration;
    }
}
