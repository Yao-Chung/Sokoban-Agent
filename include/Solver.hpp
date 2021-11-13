#ifndef SOKOBAN_AGENT_SOLVER
#define SOKOBAN_AGENT_SOLVER

#include <string>
#include <vector>
#include <random>
#include <unordered_map>

#include <defines.hpp>
#include <Visualizer.hpp>
#include <State.hpp>

class Solver{
public:
    Solver(const Map level, std::string prefix = "", std::string extension = "");
    std::vector<MoveDirection> solve();

private:
    float alpha;
    float beta;
    unsigned int maxIter;
    unsigned int boxMoveCount;
    unsigned int restartCount;
    State* root;
    const Map level;
    std::mt19937 random_generator;
    std::unordered_map<std::string, State*> states;
    std::optional<Visualizer> visualizer;

    bool isWin(const Map& map);
    void clean();
};

#endif