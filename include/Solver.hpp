#ifndef SOKOBAN_AGENT_SOLVER
#define SOKOBAN_AGENT_SOLVER

#include <string>
#include <vector>
#include <defines.hpp>

class Solver{
public:
    Solver(const Map map);
    virtual std::vector<MoveDirection> solve() = 0;
protected:
    Map getMap();
    static Map move(const Map& map, const MoveDirection direction);
private:
    const Map map;
};

#endif