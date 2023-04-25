#include<cstddef>
#include<cstdlib>
#include<iostream>
#include<string>
#include<vector>
#include"match_engine.h"

int main(const int argc,
         const char* const argv[])
{
    if(argc<2)
    {
        std::cerr<<"Usage: match_engine -I <Instruction File Path> [-A] [-E] [-M] [-O <Output File Path>] [-P] [-T <STRICT|RELAX|IGNORE>]"
                 <<std::endl;

        return EXIT_FAILURE;
    }

    const std::vector<std::string> command_line(argv+
                                                static_cast<std::ptrdiff_t>(1),
                                                argv+
                                                argc);
    if(!match_engine::configure(command_line)||
       !match_engine::process())
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
