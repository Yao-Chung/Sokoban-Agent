#ifndef SOKOBAN_AGENT_MARKLOVSOLVER
#define SOKOBAN_AGENT_MARKLOVSOLVER

#include <vector>
#include <defines.hpp>
#include <Solver.hpp>
#include <State.hpp>
#include <Action.hpp>
#include <unordered_set>

class MarklovSolver : public Solver{
public:
    unsigned int alpha;
    unsigned int beta;
    unsigned int gamma;
    unsigned int iter;
    unordered_set<State> allStates;

    std::vector<MoveDirection> solve();
    MarklovSolver(unsigned int alpha, unsigned int beta, unsigned int gamma, unsigned int iter, Map gameMap);
private:
    State curState;
    void restart();
    void update();
};

#endif