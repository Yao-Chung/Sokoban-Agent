#include <MarklovSolver/MarklovSolver.hpp>

#include <algorithm>
#include <valarray>
#include <stack>
#include <unordered_set>
#include <iostream>
#include <deque>

MarklovSolver::MarklovSolver(
    float alpha,
    float beta,
    float gamma,
    unsigned int maxIter,
    unsigned int deltaIter,
    Map map
):Solver(map), 
    alpha(alpha), 
    beta(beta), 
    gamma(gamma), 
    maxIter(maxIter), 
    deltaIter(deltaIter),
    totalBoxMoved(0),
    totalRestart(0),
    random_generator(std::random_device()())
{
}

static inline std::string getBoxKey(const std::string& key){
    return key.substr(key.find("B:"));
}

std::vector<MoveDirection> MarklovSolver::solve(){
    // initialize the root state and put into hash map
    clean();
    Map map(getMap());
    State* curState = new State(0, map);
    rootState = curState;
    allStates[curState->key] = curState;
    // walk from root state
    for(unsigned int iteration = 1; ; ++iteration){
        // create states and actions
        if(curState->actions.empty()){
            bool finished = false;
            for(MoveDirection dir: {
                    MoveDirection::Up,
                    MoveDirection::Down,
                    MoveDirection::Left,
                    MoveDirection::Right
                }){
                Map newMap = move(map, dir, getMap());
                std::string newKey = getKey(newMap);
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
                action->pathCost = 1;
                action->confidence = 0;
                action->restartCost = 0;
            }
            // Unsolved or solved
            if(finished || curState->actions.empty()){
                visualize(iteration, curState, map);
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
                curState = restart(map, iteration);
                continue;
            }
        }
        // Calculate confidence for each action
        for(Action* action: curState->actions){
            action->confidence = alpha / (float)action->pathCost;
            if(action->restartCost > 0){
                action->confidence += beta / (float)action->restartCost;
            }
            if(curState->finishTargets > 0){
                action->confidence += gamma * (float)curState->finishTargets;
            }
        }
        // Move
        Action* decision = decide(curState->actions);
        if(getBoxKey(curState->key) != getBoxKey(decision->next->key)) {
            totalBoxMoved += 1;
        }
        map = move(map, decision->direction, getMap());
        decision->pathCost += 1;
        // Update policy & current state
        policy.push(decision);
        curState = update(map, iteration, [&](){
            // visualize(iteration, curState, map);
            // if(visualizer.has_value()){
            //     visualizer->next();
            // }
        });
        visualize(iteration, curState, map);
    }
    // Transform to direction vector
    std::vector<MoveDirection> result(policy.size());
    for(auto rIt = result.rbegin(); rIt != result.rend(); rIt = std::next(rIt)){
        *rIt = policy.top()->direction;
        policy.pop();
    }
    return result;
}

void MarklovSolver::clean(){
    totalBoxMoved = 0;
    totalRestart = 0;
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
#define divide_guard(EXPR) ((EXPR > 0) ? EXPR : 0.5)

void MarklovSolver::visualize(unsigned int iteration, State* curState, const Map& map){
    if(visualizer.has_value()){
        // Print map
        for(std::string row: map){
            std::cout << row << std::endl;
        }
        // Copy policy to set
        std::stack<Action*> copied(policy);
        std::unordered_set<Action*> actionSet;
        while(!copied.empty()){
            actionSet.emplace(copied.top());
            copied.pop();
        }
        // Print
        visualizer->out << "digraph{" << std::endl;
        visualizer->out << "\t{ rank=\"min\" m" << rootState << " }" << std::endl;
        for(auto statePair: allStates){
            State* state = statePair.second;
            // Print state
            visualizer->out << "\tm" << state << "[label=\""
                << "Dis=" << state->distance << "\\n"
                << "Tar=" << state->finishTargets << "\\n"
                << "Man=(" << state->manPosition.first << ", " << state->manPosition.second << ")\\n"
            << "\"";
            if(state == curState){
                visualizer->out << ", style=filled, fillcolor=orange";
            }
            visualizer->out << "]" << std::endl;
            // Print actions
            for(Action* action: state->actions){
                visualizer->out << "\tm" << action->parent << " -> m" << action->next << "[label=\""
                    << "P=" << action->pathCost << "\\n"
                    << "R=" << action->restartCost << "\\n"
                    << "Con=" << action->confidence << "\\n";
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
        visualizer->out << "\n\tadditional[label=\""
            << "a=" << alpha << "\\n"
            << "b=" << beta << "\\n"
            << "r=" << gamma << "\\n"
            << "mI=" << maxIter << "\\n"
            << "i=" << iteration << "\\n"
        << "\",shape=note]" << std::endl;
        // Epilogue
        visualizer->out << "}" << std::endl;
    }
}

Action* MarklovSolver::decide(const std::vector<Action*> &actions){
    // Extract confidences
    std::valarray<float> possibilities(actions.size());
    std::transform(actions.begin(), actions.end(), std::begin(possibilities),
        [](Action* action){
            return action->confidence;
        }
    );
    // Calculate possibilities of each action
    possibilities /= possibilities.sum();
    // Generate random float number between 0 to 1
    float choice = std::generate_canonical<float, 32>(random_generator);
    // Make decision 
    for(size_t i = 0; i < possibilities.size(); ++i){
        choice -= possibilities[i];
        if(choice <= 0){
            return actions[i];
        }
    }
    return actions.back();
}

#define divide_guard(EXPR) ((EXPR > 0) ? EXPR : 0.5)

State* MarklovSolver::update(Map &map, unsigned int &iteration, std::function<void()> onRestart = [](){}){
    // Check dead
    State* curState = policy.top()->next;
    std::vector<Position> boxPositions = curState->boxPosition;
    bool isDead = false;
    for(Position pos: boxPositions){
        int nextToWall = 0;
        int col = pos.first, row = pos.second;
        // Check if box on target
        if(map[row][col] == '%'){
            continue;
        }
        // Check horizontal 
        if((map[row][col+1] == '#') || (map[row][col-1] == '#')){
            nextToWall += 1;
        }
        // Check vertical
        if((map[row+1][col] == '#') || (map[row-1][col] == '#')){
            nextToWall += 1;
        }
        if(nextToWall == 2){
            isDead = true;
            break;
        }
    }
    // Check iteration > maxIter  
    if(!isDead){
        if(iteration >= maxIter){
            // Update variables
            float oldAlpha = alpha;
            alpha = (float)maxIter / (float)policy.top()->pathCost;
            beta = (float)maxIter / divide_guard((float)totalRestart);
            gamma = (float)maxIter / divide_guard(
                (float)policy.top()->next->finishTargets + 
                (float)totalBoxMoved / (float)maxIter
            );
            if(alpha >= oldAlpha){
                maxIter += deltaIter;
            }
        }
        // No need to retart
        else{
            return curState;
        }
    }
    // Invoke callback
    onRestart();
    return restart(map, iteration);
}

State* MarklovSolver::restart(Map &map, unsigned int &iteration){
    // Need to restart and clean the policy stack
    while(!policy.empty()){
        Action *action = policy.top();
        policy.pop();
        action->restartCost += action->parent->distance;
    }
    // update values
    totalRestart += 1;
    totalBoxMoved = 0;
    iteration = 0;
    map = getMap();
    return rootState;
}

void MarklovSolver::attach_Visualizer(std::string prefix, std::string extention){
    visualizer.emplace(prefix, extention);
}

void MarklovSolver::PolicyStack::removeAllOf(Action* action){
    std::erase(c, action);
}