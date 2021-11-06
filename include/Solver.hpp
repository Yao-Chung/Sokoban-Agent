#ifndef SOKOBAN_AGENT_SOLVER
#define SOKOBAN_AGENT_SOLVER

#include <string>
#include <vector>
#include <defines.hpp>

class Solver{
public:
    Map map;
    Solver(const Map map);
    virtual std::vector<MoveDirection> solve() = 0;
};

#endif