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
            << "Box=" << maxIter << "\\n"
            << "Mi=" << maxIter << "\\n"
            << "i=" << iteration << "\\n"
        << "\",shape=note]" << std::endl;
        visualizer->out << "}" << std::endl;
    }
}