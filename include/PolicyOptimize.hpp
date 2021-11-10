#ifndef SOKOBAN_AGENT_POLICYOPTIMIZE
#define SOKOBAN_AGENT_POLICYOPTIMIZE

#include <vector>
#include <defines.hpp>

namespace PolicyOptimize{
    std::vector<MoveDirection> optimize(const Map map, const std::vector<MoveDirection> policy);
}

#endif