#include <Solver.hpp>

#include <string>
#include <vector>

Solver::Solver(const Map level, std::string prefix, std::string extension):
    level(level)
{
    if(!prefix.empty() || !extension.empty()){
        visualizer.emplace(prefix, extension);
    }
}

bool Solver::isWin(const Map& map){
    for(std::string row: map){
        if(row.find('$') != std::string::npos){
            return false;
        }
    }
    return true;
}