#ifndef SOKOBAN_AGENT_STATE
#define SOKOBAN_AGENT_STATE

#include <vector>
#include <unordered_set>
#include <defines.hpp>

struct State;

struct Action{
    unsigned int pathCost;
    unsigned int restartCost;
    float confidence;
    MoveDirection direction;
    State *parent;
    State *next;
};

struct State{
    std::unordered_set<State*> parents;
    std::vector<Action*> actions;

    unsigned int distance;
    unsigned int finishTargets;
    Position manPosition;
    std::vector<Position> boxPosition;
    std::string key;

    State() = default;
    State(unsigned int distance, Map map);
    ~State();
};

#endif