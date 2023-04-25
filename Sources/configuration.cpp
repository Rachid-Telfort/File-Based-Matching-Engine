#include"configuration.h"

configuration::configuration():_errors(),
                               _instruction_file_path(),
                               _output_file_path(),
                               _timestamp_configuration(timestamp_configuration::STRICT),
                               _ignore_errors(),
                               _insert_match(),
                               _process_errors(),
                               _strict_amend(){}
