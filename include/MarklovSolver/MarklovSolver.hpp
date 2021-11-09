#ifndef SOKOBAN_AGENT_MARKLOVSOLVER
#define SOKOBAN_AGENT_MARKLOVSOLVER

#include <stack>
#include <random>
#include <optional>
#include <defines.hpp>
#include <Solver.hpp>
#include <MarklovSolver/State.hpp>
#include <unordered_map>
#include <Visualizer.hpp>

class MarklovSolver : public Solver{
public:
    float alpha;
    float beta;
    float gamma;
    unsigned int maxIter;
    unsigned int deltaIter;
    unsigned int totalBoxMoved;
    unsigned int totalRestart;
    std::unordered_map<std::string, State*> allStates;
    std::stack<Action*> policy;
    std::stack<MoveDirection> solve();
    void attach_Visualizer(std::string prefix, std::string extention);
    MarklovSolver(
        float alpha, float beta, float gamma, 
        unsigned int iter, unsigned int deltaIter,
        Map map
    );
private:
    std::mt19937 random_generator;
    void clean();
    void visualize(unsigned int iteration, State* curState, const Map& map);
    State* update(const Map &map, unsigned int &iteration);
    Action* decide(const std::vector<Action*> &actions);
    std::optional<Visualizer> visualizer;
};

#endif