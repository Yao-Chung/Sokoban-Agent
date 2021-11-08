#ifndef SOKOBAN_AGENT_MARKLOVSOLVER
#define SOKOBAN_AGENT_MARKLOVSOLVER

#include <vector>
#include <defines.hpp>
#include <Solver.hpp>
#include <MarklovSolver/State.hpp>
#include <unordered_map>

class MarklovSolver : public Solver{
public:
    float alpha;
    float beta;
    float gamma;
    unsigned int iter;
    std::unordered_map<std::string, State*> allStates;

    std::vector<MoveDirection> solve();
    MarklovSolver(float alpha, float beta, float gamma, unsigned int iter, Map map);
private:
    State* restart();
    void update();
};

#endif