#include <MarklovSolver/State.hpp>

State::State(unsigned int distance, Map map):
    distance(distance), finishTargets(0)
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
    for(Action* action: actions){
        delete action;
    }
}