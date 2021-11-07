#ifndef SOKOBAN_AGENT_STATE
#define SOKOBAN_AGENT_STATE

#include <vector>
#include <defines.hpp>

struct State;

struct Action{
    unsigned int pathCost;
    unsigned int restartCost;
    MoveDirection direction;
    State *parent;
    State *next;
};

struct State{
    std::vector<State*> parents;
    std::vector<Action> actions;
    
    unsigned int distance;
    unsigned int finishTargets;
    Position manPosition;
    std::vector<Position> boxPosition;
    std::string key;

    State() = default;
    State(unsigned int distance, Map map);
    static std::string getKey(const Map map);
    static std::string getKey(const Position manPos, const std::vector<Position> boxPos);
    static std::pair< Position, std::vector<Position> > getPositions(const Map map);
};

#endif