#include <Solver.hpp>

#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <unordered_map>

#include <State.hpp>

Solver::Solver(const Map level, std::string prefix, std::string extension):
    alpha(1),
    beta(1),
    maxIter(1),
    boxMoveCount(0),
    restartCount(0),
    root(nullptr),
    level(level),
    random_generator(std::random_device()())
{
    // Attach visualizer
    if(!prefix.empty() || !extension.empty()){
        visualizer.emplace(prefix, extension);
    }
}

Solver::~Solver(){
    delete root;
}

static inline std::string getBoxKey(const std::string& key){
    return key.substr(key.find("B:"));
}

std::vector<MoveDirection> Solver::solve(){
    clean();
    // Initialize current map and current state
    Map map = level;
    State* curState = new State(0, map, nullptr, MoveDirection::Unspecified);
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
                    State* nextState = new State(curState->distance+1, newMap, curState, dir);
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
    // $ shouldn't exist
    for(std::string row: map){
        if(row.find('$') != std::string::npos){
            return false;
        }
    }
    return true;
}

Decimal Solver::confidence(const State* const state){
    // a / R + b / T
    return (alpha / (Decimal) state->restartCost) + (beta / (Decimal)state->finishTargets);
}

State* Solver::restart(Map &map, unsigned int &iteration){
    boxMoveCount = 0;
    restartCount += 1;
    iteration = 0;
    map = level;
    return root;
}

MoveDirection Solver::decide(const State* const state){
    // Calculate confidences and sum
    std::vector<std::pair<Decimal, MoveDirection>> possibilities;
    Decimal sum = 0;
    for(auto [direction, child]: state->childs){
        std::pair<Decimal, MoveDirection> &pair = possibilities.emplace_back(confidence(child), direction);
        sum += pair.first;
    }
    // Generate random float number between 0 to 1
    Decimal choice = std::generate_canonical<Decimal, sizeof(Decimal) * 8>(random_generator);
    // Make decision 
    for(auto [conf, dir]: possibilities){
        choice -= conf;
        if(choice <= 0){
            return dir;
        }
    }
    return possibilities.back().second;
}

void Solver::visualize(const unsigned int iteration, const State* const curState, const Map& map){
    if(visualizer.has_value()){
        // Print map
        for(std::string row: map){
            std::cout << row << std::endl;
        }
        // Copy policy to set
        std::unordered_map<State*, MoveDirection> policy;
        for(const State* cursor = curState; cursor != nullptr && cursor->parent != nullptr; cursor = cursor->parent){
            for(auto [direction, child]: cursor->parent->childs){
                if(child == cursor){
                    policy.emplace(cursor->parent, direction);
                }
            }
        }
        // Prologue
        visualizer->out << std::string("digraph{") << std::endl;
        // Print states by BFS
        std::queue<State*> stateQueue;
        stateQueue.push(root);
        while(!stateQueue.empty()){
            State* state = stateQueue.front();
            stateQueue.pop();
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
            for(auto [direction, child]: state->childs){
                visualizer->out << "\tm" << state << " -> m" << child << "[label=\"";
                switch (direction){
                case MoveDirection::Up:
                    visualizer->out << "Up";
                    break;
                case MoveDirection::Down:
                    visualizer->out << "Down";
                    break;
                case MoveDirection::Left:
                    visualizer->out << "Left";
                    break;
                case MoveDirection::Right:
                    visualizer->out << "Right";
                    break;
                default:
                break;
                }
                visualizer->out << "(" << confidence(child) << ")\"";
                // Color direction in policy
                if(policy.contains(state) && (policy[state] == direction)){
                    visualizer->out << ", color=red";
                }
                visualizer->out << "]" << std::endl;
            }
        }
        // Epilogue
        visualizer->out << "\n\tadditional[label=\""
            << "alpha =" << alpha << "\\n"
            << "beta =" << beta << "\\n"
            << "BoxMoveC=" << boxMoveCount << "\\n"
            << "RestartC=" << restartCount << "\\n"
            << "Mi=" << maxIter << "\\n"
            << "i=" << iteration << "\\n"
        << "\",shape=note]" << std::endl;
        visualizer->out << "}" << std::endl;
    }
}