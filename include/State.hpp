#ifndef SOKOBAN_AGENT_STATE
#define SOKOBAN_AGENT_STATE

#include <vector>
#include <unordered_map>
#include <defines.hpp>

struct State{
    State* parent;
    std::unordered_map<MoveDirection, State*> childs;
    unsigned int distance;
    unsigned int restartCost;
    unsigned int finishTargets;
    Position manPosition;
    std::vector<Position> boxPosition;
    std::string key;

    State() = default;
    State(unsigned int distance, Map map, State* parent);
    ~State();
};

#endif