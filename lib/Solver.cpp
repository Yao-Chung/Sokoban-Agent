#include <Solver.hpp>
#include <State.hpp>
#include <string>
#include <vector>

Solver::Solver(const Map level, std::string prefix, std::string extension):
    level(level)
{
    if(!prefix.empty() || !extension.empty()){
        visualizer.emplace(prefix, extension);
    }
}

static inline std::string getBoxKey(const std::string& key){
    return key.substr(key.find("B:"));
}

std::vector<MoveDirection> Solver::solve(){
    State* curState = new State(0, level, nullptr);
    root = curState;
    states[curState->key] = curState;
    for(MoveDirection dir: {
                    MoveDirection::Up,
                    MoveDirection::Down,
                    MoveDirection::Left,
                    MoveDirection::Right
                }){
                
    
    }
}

void Solver::clean(){
    boxMoveCount = 0;
    restartCount = 0;
    // Clean allStates
    delete root;
    states.clear();
}

bool Solver::isWin(const Map& map){
    for(std::string row: map){
        if(row.find('$') != std::string::npos){
            return false;
        }
    }
    return true;
}