#ifndef SOKOBAN_AGENT_SOLVER
#define SOKOBAN_AGENT_SOLVER

#include <string>
#include <stack>
#include <defines.hpp>

class Solver{
public:
    Solver(const Map map);
    virtual std::stack<MoveDirection> solve() = 0;
protected:
    Map getMap();
    Map move(const Map& map, const MoveDirection direction);
    bool isWin(const Map& map);
private:
    const Map map;
};

#endif