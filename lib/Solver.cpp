#include <Solver.hpp>

#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <State.hpp>

#define visualizer_advance 1

Solver::Solver(const Map level, std::string cnnPath, std::string prefix, std::string extension):
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
    if(cnnPath.empty()){
        gamma = 0;
    }else{
        trainer.emplace(cnnPath);
        gamma = 1;
    }
}

Solver::~Solver(){
    delete root;
}

static inline std::string getBoxKey(const std::string& key){
    return key.substr(key.find("B:"));
}

static void printMap(const Map &map){
    // Print map
    for(std::string row: map){
        std::cout << row << std::endl;
    }
    std::cout << std::endl;
}

std::vector<MoveDirection> Solver::solve(){
    clean();
    // Initialize current map and current state
    Map map = level;
    State* curState = new State(0, map, nullptr, MoveDirection::Unspecified);
    root = curState;
    states[curState->key] = curState;
    // Visualize
    visualize(0, curState, map);
    for(unsigned int iteration=1; ; ++iteration){
        // Check fresh state
        if(curState->childs.empty()){
            // Get suggestion
            std::vector<Decimal> suggestions;
            if(trainer.has_value()){
                suggestions = trainer->suggest(map);
            }
            
            // Make actions
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
                    if((key == curState->key) || (curState->parent->key == key)){
                        continue;
                    }
                    // Other state
                    State *nextState = states[key];
                    if(trainer.has_value()){
                        nextState->suggestion = suggestions[dir];
                    }
                    if(nextState->parent != nullptr){
                        nextState->distance = nextState->parent->distance + 1;
                    }
                    // A better way to next state
                    if(curState->distance + 1 < nextState->distance){
                        // Cut the relation of next state and its parent
                        State *nextParent = nextState->parent;
                        nextParent->childs.erase(nextState->direction);
                        // Change the parent of next state
                        nextState->parent = curState;
                        nextState->direction = dir;
                        nextState->distance = curState->distance + 1;
                        // Add next state to current state's childs
                        curState->childs[dir] = nextState;
                    }
                }
                // It's a new state            
                else{
                    // Create new state and put into states
                    State* nextState = new State(curState->distance + 1, newMap, curState, dir);
                    if(trainer.has_value()){
                        nextState->suggestion = suggestions[dir];
                    }
                    curState->childs[dir] = nextState;
                    states[nextState->key] = nextState;
                    // Check if win or not
                    if(isWin(newMap)){
                        // Visualize
                        visualize(iteration, curState, newMap);
                        // Calculate the winning path from curState
                        curState = nextState;
                        std::vector<MoveDirection> policy;
                        while(curState != root){
                            policy.emplace_back(curState->direction);
                            curState = curState->parent;
                        }
                        std::reverse(policy.begin(), policy.end());
                        return policy;
                    }
                }
            }
            // Check if curState is dead node
            if(curState->childs.empty()){
                Decimal sugSum = 0;
                size_t count = 0;
                // Clean dead nodes
                while (curState->childs.empty()){
                    if(curState->parent == nullptr){
                        // Unsolved
                        return {};
                    }else{
                        if(trainer.has_value()){
                            sugSum += curState->suggestion;
                            count += 1;
                        }
                        State* parent = curState->parent;
                        parent->childs.erase(curState->direction);
                        states.erase(curState->key);
                        delete curState;
                        curState = parent;
                    }
                }
                // Restart
                curState = restart(map, iteration, curState);
                // Update gamma
                if(trainer.has_value()){
                    gamma = 1.0 - (sugSum / (Decimal)count);
                }
                continue;
            }
        }
        // Decide which direction to go
        MoveDirection nextDir = decide(curState, map);
        State* nextState = curState->childs[nextDir];
        // Update boxMoveCount
        if(getBoxKey(curState->key) != getBoxKey(nextState->key)){
            boxMoveCount += 1;
        }
        // Update finishedTargets
        if(curState->finishTargets < nextState->finishTargets){
            for(State* cur = curState; cur != nullptr; cur = cur->parent){
                if(cur->finishTargets < nextState->finishTargets){
                    cur->finishTargets = nextState->finishTargets;
                }
            }
        }
        // Move to the next state
        curState = nextState;
        map = move(map, nextDir, level);
        // Check if reach the maxIter
        if(iteration >= maxIter){
            // Update alpha, beta, maxIter
            alpha = (Decimal)std::sqrt(restartCount + 1);
            beta = ((curState->finishTargets == 0) ? 1 : ((Decimal)maxIter / (Decimal)curState->finishTargets)) + (Decimal)boxMoveCount / (Decimal)maxIter;
            maxIter += 1;
            std::cerr << "MaxIter: " << maxIter << " Restart: " << restartCount
                << " a: " << alpha
                << " b: " << beta 
                << " r: " << gamma 
                << " R: " << curState->restartCost
                << " a/R: " << ((curState->restartCost > 0) ? (alpha / (Decimal)curState->restartCost) : 1)
                << " b*T: " << beta * (Decimal)curState->finishTargets
                << std::endl; 
            // Visualize
            visualize(0, curState, map);
            curState = restart(map, iteration, curState);
        }
    }
}

void Solver::clean(){
    boxMoveCount = 0;
    restartCount = 0;
    alpha = 1;
    beta = 1;
    maxIter = 1;
    delete root;
    root = nullptr;
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
    // a / R + b * T
    Decimal result = (state->restartCost > 0) ? (alpha / (Decimal) state->restartCost) : 1;
    if(state->finishTargets > 0){
        result += beta * (Decimal)state->finishTargets;
    }
    return result;
}

State* Solver::restart(Map &map, unsigned int &iteration, State* curState){
    while(curState != nullptr){
        curState->restartCost += curState->distance;
        curState = curState->parent;
    }
    boxMoveCount = 0;
    restartCount += 1;
    iteration = 0;
    map = level;
    return root;
}

MoveDirection Solver::decide(const State* const state, const Map map){
    // Calculate confidences and sum
    std::vector<std::pair<Decimal, MoveDirection>> possibilities;
    std::vector<Decimal> suggestions(4);

    Decimal sum = 0;
    Decimal expert_sum = 0;
    for(auto [direction, child]: state->childs){
        std::pair<Decimal, MoveDirection> &pair = possibilities.emplace_back(confidence(child), direction);
        sum += pair.first;
        expert_sum += child->suggestion;
        if(trainer.has_value()){
            suggestions[direction] = child->suggestion;
        }
    }
    // Generate random float number between 0 to 1
    Decimal choice = std::generate_canonical<Decimal, sizeof(Decimal) * 8>(random_generator);
    // Make decision 
    for(auto [conf, dir]: possibilities){
        choice -= (1 - gamma) * (conf / sum);
        if(trainer.has_value()){
            choice -= gamma * (suggestions[dir] / expert_sum);
        }
        if(choice <= 0){
            return dir;
        }
    }
    return possibilities.back().second;
}

void Solver::visualize(const unsigned int iteration, const State* const curState, const Map& map){
    if(visualizer.has_value()){
        // Copy policy to set
        std::unordered_set<const State*> policy;
        for(const State* cursor = curState; cursor != nullptr; cursor = cursor->parent){
            policy.insert(cursor);
        }

        // Prologue
        visualizer->next(visualizer_advance);
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
                << "S=" << state->suggestion << "\\n"
                << "Pos=(" << state->manPosition.first << ", " << state->manPosition.second << ")\\n"
            << "\"";
            if(state == curState){
                visualizer->out << ", style=filled, fillcolor=orange";
            }
            visualizer->out << "]" << std::endl;
            // Print actions
            for(auto [direction, child]: state->childs){
                stateQueue.push(child);
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
                if(policy.contains(child)){
                    visualizer->out << ", color=red";
                }
                visualizer->out << "]" << std::endl;
            }
        }
        // Epilogue
        visualizer->out << "\n\tadditional[label=\""
            << "alpha =" << alpha << "\\n"
            << "beta =" << beta << "\\n"
            << "gamma =" << gamma << "\\n"
            << "BoxMoveC=" << boxMoveCount << "\\n"
            << "RestartC=" << restartCount << "\\n"
            << "Mi=" << maxIter << "\\n"
            << "i=" << iteration << "\\n"
        << "\",shape=note]" << std::endl;
        visualizer->out << "}" << std::endl;
    }
}