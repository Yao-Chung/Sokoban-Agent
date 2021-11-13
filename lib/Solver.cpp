#include <Solver.hpp>
#include <State.hpp>
#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <unordered_map>

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
    clean();
    // Initialize current map and current state
    Map map = level;
    State* curState = new State(0, map, nullptr);
    root = curState;
    states[curState->key] = curState;
    bool finished = false;
    for(unsigned int iteration=1; !finished; ++iteration){
        for(MoveDirection dir: {
            MoveDirection::Up,
            MoveDirection::Down,
            MoveDirection::Left,
            MoveDirection::Right
        }){
                Map newMap = move(map, dir, level);
                std::string key = getKey(newMap);
                // State exists
                if(states.contains(key)){
                    // It is parent or hit the wall (itself)
                    if(curState->parent == states[key] || key == curState->key)
                        continue;
                    // Other state
                    State *nextState = states[key];
                    // A better way to next state
                    if(curState->distance+1 < nextState->distance){
                        // Cut the relation of next state and its parent
                        State *nextStateParent = nextState->parent;
                        for(auto p: nextStateParent->childs){
                            if(p.second == nextState){
                                nextStateParent->childs.erase(p.first);
                                break;
                            }
                        }
                        // Change the parent of next state
                        nextState->parent = curState;
                        // Add next state to current state's childs
                        curState->childs[dir] = nextState;
                    }
                }
                // It's a new state            
                else{
                    // Create new state and put into states
                    State* nextState = new State(curState->distance+1, newMap, curState);
                    curState->childs[dir] = nextState;
                    states[nextState->key] = nextState;
                    // Check if win or not
                    if(isWin(newMap)){
                        // Let curState be the winning state
                        finished = true;
                        curState = nextState;
                        break;
                    }
                }
        }
        // Get into a dead node
        if(curState->childs.empty()){
            curState = restart(map, iteration);
            continue;
        }
        // Decide which direction to go
        MoveDirection nextDir = decide(curState);
        // Move to the next state
        curState = curState->childs[nextDir];
        map = move(map, nextDir, level);
        // Check if reach the maxIter
        if(iteration >= maxIter){
            // Update alpha, beta, maxIter
            alpha = (Decimal)maxIter / (Decimal)restartCount;
            beta = (Decimal)maxIter / (Decimal)curState->finishTargets + (Decimal)boxMoveCount / (Decimal)maxIter;
            maxIter += 1;
            curState = restart(map, iteration);
        }
    }
    // Calculate the winning path from curState
    std::vector<MoveDirection> policy;
    
    return policy;
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

Decimal Solver::confidence(const State* const state){
    return (alpha / (Decimal) state->restartCost) + (beta / (Decimal)state->finishTargets);
}

State* Solver::restart(Map &map, unsigned int &iteration){
    boxMoveCount = 0;
    restartCount += 1;
    iteration = 0;
    map = level;
    return root;
}

void Solver::visualize(unsigned int iteration, State* curState, const Map& map){
    if(visualizer.has_value()){
        // Print map
        for(std::string row: map){
            std::cout << row << std::endl;
        }
        // Copy policy to set
        std::unordered_map<State*, MoveDirection> policy;
        for(State* cursor = curState; cursor != nullptr && cursor->parent != nullptr; cursor = cursor->parent){
            for(auto child: cursor->parent->childs){
                if(child.second.first == cursor){
                    policy.emplace(cursor->parent, child.first);
                }
            }
        }
        // Prologue
        visualizer->out << "digraph{" << std::endl;
        // Print states by BFS
        std::queue<State*> stateQueue;
        stateQueue.push(root);
        while(!stateQueue.empty()){
            State* state = stateQueue.front();
            // Print state
            visualizer->out << "\tm" << state << "[label=\""
                << "D=" << state->distance << "\\n"
                << "T=" << state->finishTargets << "\\n"
                << "R=" << state->restartCost << "\\n"
                << "Pos=(" << state->manPosition.first << ", " << state->manPosition.second << ")\\n"
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
        // Epilogue
        visualizer->out << "\n\tadditional[label=\""
            << "alpha =" << alpha << "\\n"
            << "beta =" << beta << "\\n"
            << "Box=" << maxIter << "\\n"
            << "Mi=" << maxIter << "\\n"
            << "i=" << iteration << "\\n"
        << "\",shape=note]" << std::endl;
        visualizer->out << "}" << std::endl;
    }
}