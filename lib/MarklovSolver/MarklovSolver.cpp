#include <MarklovSolver/MarklovSolver.hpp>
#include <queue>
#include <optional>

MarklovSolver::MarklovSolver(
    float alpha,
    float beta,
    float gamma,
    unsigned int iter,
    Map map
): Solver(map), alpha(alpha), beta(beta), gamma(gamma), iter(iter){

}

static inline std::string getBoxKey(const std::string& key){
    return key.substr(key.find("B:"));
}

std::vector<MoveDirection> MarklovSolver::solve(){
    // Variables
    std::vector<MoveDirection> result;
    std::optional<MoveDirection> lastDirection;
    // initialize the root state and put into hash map
    Map map(getMap());
    State* curState = new State(0, map);
    allStates[curState->key] = curState;
    // walk from root state
    while(true){
        if(curState->actions.empty()){
            // create states and actions
            bool finished = false;
            for(MoveDirection dir: {
                    MoveDirection::Up,
                    MoveDirection::Down,
                    MoveDirection::Left,
                    MoveDirection::Right
                }){
                Map newMap = move(map, dir);
                std::string newKey = State::getKey(newMap);
                // check if is win or not
                if(isWin(newMap)){
                    finished = true;
                    break;
                }
                // check if newState equals to curState (hit wall or hit box)
                if(curState->key == newKey){
                    continue;
                }
                // Check if state exist or not
                if(allStates.contains(newKey)){
                    allStates[newKey]->parents.emplace(curState);
                }else{
                    allStates[newKey] = new State(curState->distance + 1, newMap);
                }
                State* nextState = allStates[newKey];
                // Create curState's action
                Action& action = curState->actions.emplace_back();
                action.parent = curState;
                action.next = nextState;
                action.direction = dir;
                action.pathCost = 1;
                action.restartCost = (getBoxKey(curState->key) == getBoxKey(nextState->key)) ? 0 : curState->distance;
            }
            // Unsolved or solved
            if(finished || curState->actions.empty()){
                break;
            }
        }
        // Calculate confidence for each action
        // Decide action
        // Update map to newMap
        // Increase iteration & check iter
    }
    return result;
}

State* MarklovSolver::restart(){

}

void MarklovSolver::update(){

}