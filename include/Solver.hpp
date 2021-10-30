#ifndef SOKOBAN_AGENT_SOLVER_DEF
#define SOKOBAN_AGENT_SOLVER_DEF

#include <string>

class Solver{
public:
    std::string level;
    Solver(const std::string level);
    bool solve();
};

#endif