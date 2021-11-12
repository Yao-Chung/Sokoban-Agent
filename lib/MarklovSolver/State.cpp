#include <MarklovSolver/State.hpp>

State::State(unsigned int distance, Map map, State* parent):
    parent(parent), distance(distance), restartCost(0), finishTargets(0)
{
    std::pair< Position, std::vector<Position> > positions(getPositions(map));
    manPosition = positions.first;
    boxPosition = positions.second;
    key = getKey(manPosition, boxPosition);
    for(std::string row: map){
        for(char elem: row){
            if(elem == '%'){
                finishTargets += 1;
            }
        }
    }
}

State::~State(){
    for(auto p: childs){
        delete p.second.first;
    }
}