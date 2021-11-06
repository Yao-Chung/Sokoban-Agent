#include <Solver.hpp>

Solver::Solver(const Map map):
    map(map)
{ 
}

Map Solver::getMap(){
    return map;
}

Map Solver::move(const Map& map, const MoveDirection direction){
    Map result(map);
    // TODO:
    return result;
}