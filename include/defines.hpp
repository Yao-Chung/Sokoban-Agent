#ifndef SOKOBAN_AGENT_DEFINES
#define SOKOBAN_AGENT_DEFINES

#include <vector>
#include <string>

enum class MoveDirection{
    Left,
    Right,
    Up,
    Down,
};

using Map = std::vector<std::string>;

#endif