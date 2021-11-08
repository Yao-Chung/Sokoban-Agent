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
    unsigned int iter;
    std::unordered_map<std::string, State*> allStates;

    std::stack<MoveDirection> solve();
    void attach_Visualizer(std::string prefix, std::string extention);
    MarklovSolver(float alpha, float beta, float gamma, unsigned int iter, Map map);
private:
    State* restart();
    void update();
    std::optional<Visualizer> visualizer;
};

#endif