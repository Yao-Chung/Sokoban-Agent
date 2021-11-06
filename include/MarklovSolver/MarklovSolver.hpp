#ifndef SOKOBAN_AGENT_MARKLOVSOLVER
#define SOKOBAN_AGENT_MARKLOVSOLVER

#include <vector>
#include <defines.hpp>
#include <Solver.hpp>
#include <MarklovSolver/State.hpp>
#include <unordered_map>

class MarklovSolver : public Solver{
public:
    unsigned int alpha;
    unsigned int beta;
    unsigned int gamma;
    unsigned int iter;
    std::unordered_map<std::string, State> allStates;

    std::vector<MoveDirection> solve();
    MarklovSolver(unsigned int alpha, unsigned int beta, unsigned int gamma, unsigned int iter, Map map);
private:
    State curState;
    void restart();
    void update();
};

#endif