#ifndef SOKOBAN_AGENT_MARKLOVSOLVER
#define SOKOBAN_AGENT_MARKLOVSOLVER

#include <stack>
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
    void clean();
    State* restart();
    std::optional<Visualizer> visualizer;
};

#endif