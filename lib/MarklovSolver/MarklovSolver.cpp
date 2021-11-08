#include <MarklovSolver/MarklovSolver.hpp>

#include <algorithm>
#include <stack>
#include <unordered_set>

MarklovSolver::MarklovSolver(
    float alpha,
    float beta,
    float gamma,
    unsigned int maxIter,
    unsigned int deltaIter,
    Map map
): Solver(map), alpha(alpha), beta(beta), gamma(gamma), maxIter(maxIter), deltaIter(deltaIter){

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
        // Visualize
        visualize(iteration, curState);
        // create states and actions
        if(curState->actions.empty()){
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
        policy.push(max_action);
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

void MarklovSolver::visualize(unsigned int iteration, State* curState){
    if(visualizer.has_value()){
        // Copy policy to set
        std::stack copied = policy;
        std::unordered_set<Action*> actionSet;
        while(!copied.empty()){
            actionSet.emplace(copied.top());
            copied.pop();
        }
        // Get root state
        State* root = allStates[State::getKey(getMap())];
        // Print
        visualizer->out << "digraph{" << std::endl;
        visualizer->out << "\t{ rank=\"min\" m" << root << " }" << std::endl;
        for(auto statePair: allStates){
            State* state = statePair.second;
            // Print state
            visualizer->out << "\tm" << state << "[label=\""
                << "Dis=" << state->distance << "\\n"
                << "Tar=" << state->finishTargets << "\\n"
                << "Man=(" << state->manPosition.first << ", " << state->manPosition.second << ")\\n"
            << "\"";
            if(state == curState){
                visualizer->out << ",color=blue";
            }
            visualizer->out << "]" << std::endl;
            // Print actions
            for(Action* action: state->actions){
                visualizer->out << "\tm" << action->parent << " -> m" << action->next << "[label=\""
                    << "P=" << action->pathCost << "\\n"
                    << "R=" << action->restartCost << "\\n";
                switch (action->direction){
                case MoveDirection::Up:
                    visualizer->out << "Dir=Up\\n";
                    break;
                case MoveDirection::Down:
                    visualizer->out << "Dir=Down\\n";
                    break;
                case MoveDirection::Left:
                    visualizer->out << "Dir=Left\\n";
                    break;
                case MoveDirection::Right:
                    visualizer->out << "Dir=Right\\n";
                    break;
                }
                visualizer->out << "\"";
                // Color actions in policy
                if(actionSet.contains(action)){
                    visualizer->out << ", color=red";
                }
                visualizer->out << "]" << std::endl;
            }
        }
        // Print additional values
        visualizer->out << "additional[label=\""
            << "a=" << alpha << "\\n"
            << "b=" << beta << "\\n"
            << "r=" << gamma << "\\n"
            << "mI=" << maxIter << "\\n"
            << "i=" << iteration << "\\n"
        << "\",shape=note]" << std::endl;
        // Epilogue
        visualizer->out << "}" << std::endl;
        visualizer->next();
    }
}

#define divide_guard(EXPR) ((EXPR > 0) ? EXPR : 1e-6)

State* MarklovSolver::restart(){
    // Update variables
    float oldAlpha = alpha;
    alpha = maxIter / policy.top()->pathCost;
    beta = maxIter / divide_guard(totalBoxMoved);
    gamma = (maxIter / divide_guard(policy.top()->next->finishTargets)) + (totalBoxMoved/maxIter);
    if(alpha >= oldAlpha){
        maxIter += deltaIter;
    }
    // Clean policies & totalBoxMove
    totalBoxMoved = 0;
    while(!policy.empty()){
        policy.pop();
    }
    return allStates[State::getKey(getMap())];
}

void MarklovSolver::attach_Visualizer(std::string prefix, std::string extention){
    visualizer.emplace(prefix, extention);
}