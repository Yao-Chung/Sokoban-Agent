#include <Solver.hpp>

#include <string>
#include <vector>

Solver::Solver(const Map map):
    map(map)
{ 
}

Map Solver::getMap(){
    return map;
}

bool Solver::isWin(const Map& map){
    for(std::string row: map){
        if(row.find('$') != std::string::npos){
            return false;
        }
    }
    return true;
}