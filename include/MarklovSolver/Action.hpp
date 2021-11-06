#ifndef SOKOBAN_AGENT_ACTION
#define SOKOBAN_AGENT_ACTION

#include <State.hpp>
#include <defines.hpp>

class Action{
public:
    unsigned int pathCost;
    unsigned int restartCost;
    MoveDirection direction;
    State *parent;
    State *next;
};

#endif