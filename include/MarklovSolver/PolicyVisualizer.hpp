#ifndef SOKOBAN_AGENT_MARKLOVSOLVER_POLICYVISUALIZER
#define SOKOBAN_AGENT_MARKLOVSOLVER_POLICYVISUALIZER

#include <vector>
#include <defines.hpp>
#include <Visualizer.hpp>
#include <MarklovSolver/State.hpp>

struct PolicyVisualizer: public Visualizer{
    PolicyVisualizer(std::string prefix, std::string extension);
    void visualize(
        const std::vector<MoveDirection> &policy,
        const State* root
    );
};

#endif