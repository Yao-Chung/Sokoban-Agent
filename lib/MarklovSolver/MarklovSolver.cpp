#include <MarklovSolver/MarklovSolver.hpp>

#include <algorithm>

MarklovSolver::MarklovSolver(
    float alpha,
    float beta,
    float gamma,
    unsigned int maxIter,
    Map map
): Solver(map), alpha(alpha), beta(beta), gamma(gamma), maxIter(maxIter){

}

static inline std::string getBoxKey(const std::string& key){
    return key.substr(key.find("B:"));
}

std::stack<MoveDirection> MarklovSolver::solve(){
    // initialize the root state and put into hash map
    clean();
    Map map(getMap());
    State* curState = new State(0, map);
    allStates[curState->key] = curState;
    // walk from root state
    for(unsigned int iteration = 1; ; ++iteration){
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
                Action* action = curState->actions.emplace_back(new Action());
                action->parent = curState;
                action->next = nextState;
                action->direction = dir;
                action->pathCost = 0;
                if(getBoxKey(curState->key) == getBoxKey(nextState->key)) {
                    action->restartCost = 0;   
                }else{
                    action->restartCost = curState->distance;
                    totalBoxMoved += 1;
                }
            }
            // Unsolved or solved
            if(finished || curState->actions.empty()){
                break;
            }
        }
        // Check if no further action without backward
        if(curState->actions.size() == 1 && !policy.empty()){
            bool isBackWard = false;
            switch (curState->actions.back()->direction){
            case MoveDirection::Up:
                isBackWard = (policy.top()->direction == MoveDirection::Down);
                break;
            case MoveDirection::Down:
                isBackWard = (policy.top()->direction == MoveDirection::Up);
                break;
            case MoveDirection::Left:
                isBackWard = (policy.top()->direction == MoveDirection::Right);
                break;
            case MoveDirection::Right:
                isBackWard = (policy.top()->direction == MoveDirection::Left);
                break;
            }
            if(isBackWard){
                // Move
                map = move(map, curState->actions.back()->direction);
                // Clean
                State* backState = policy.top()->parent;
                backState->actions.erase(std::find(backState->actions.begin(), backState->actions.end(), policy.top()));
                allStates.erase(curState->key);
                delete curState;
                curState = backState;
                delete policy.top();
                policy.pop();
                continue;
            }
        }
        // Calculate confidence for each action and Decide action
        float max_confidence = -1.0f;
        Action *max_action = nullptr;
        for(Action* action: curState->actions){
            float cur_confidence = (alpha/action->pathCost) +(beta/action->restartCost) + (gamma*curState->finishTargets);
            if(cur_confidence > max_confidence){
                max_confidence = cur_confidence;
                max_action = action;
            }
        }
        // Update map to newMap
        curState = max_action->next;
        map = move(map, max_action->direction);
        max_action->pathCost += 1;
        // Increase iteration & check iter
        if(iteration >= maxIter){
            iteration = 0;
            curState = restart();
        }
    }
    // Transform to direction vector
    std::stack<MoveDirection> result;
    while(!policy.empty()){
        result.push(policy.top()->direction);
        policy.pop();
    }
    return result;
}

void MarklovSolver::clean(){
    totalBoxMoved = 0;
    // Clean allStates
    for(auto statePair: allStates){
        delete statePair.second;
    }
    allStates.clear();
    // Clean policy
    while(!policy.empty()){
        policy.pop();
    }
}

State* MarklovSolver::restart(){
    alpha = maxIter/policy.top()->pathCost;
    beta = maxIter/
}

void MarklovSolver::attach_Visualizer(std::string prefix, std::string extention){
    visualizer.emplace(prefix, extention);
}