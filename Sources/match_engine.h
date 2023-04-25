#ifndef MATCH_ENGINE_H_INCLUDED
#define MATCH_ENGINE_H_INCLUDED

#include<string>
#include<vector>

namespace match_engine
{
    bool configure(const std::vector<std::string>&);
    bool process();
}

#endif
