#ifndef SOKOBAN_AGENT_STATE
#define SOKOBAN_AGENT_STATE

#include <vector>
#include <MarklovSolver/Action.hpp>
#include <defines.hpp>

class State{
public:
    std::vector<State*> parents;
    std::vector<Action> actions;
    unsigned int distance;
    unsigned int finishTargets;
    std::pair<int, int> manPosition;
    std::vector< std::pair<int, int> > boxPosition;
    State(unsigned int distance, Map gameMap);
};

#endif