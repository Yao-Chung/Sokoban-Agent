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
    ~Solver();

private:
    Decimal alpha;
    Decimal beta;
    unsigned int maxIter;
    unsigned int boxMoveCount;
    unsigned int restartCount;
    State* root;
    const Map level;
    std::mt19937 random_generator;
    std::unordered_map<std::string, State*> states;
    std::optional<Visualizer> visualizer;

    bool isWin(const Map& map);
    Decimal confidence(const State* const state);
    MoveDirection decide(const State* const state);
    void visualize(const unsigned int iteration, const State* const curState, const Map& map);
    void clean();
    State* restart(Map &map, unsigned int &iteration, State* curState);
};

#endif