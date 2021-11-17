#ifndef SOKOBAN_AGENT_DEFINES
#define SOKOBAN_AGENT_DEFINES

#include <vector>
#include <string>

enum MoveDirection{
    Left = 0,
    Right = 1,
    Up = 2,
    Down = 3,
    Unspecified = -1,
};

using Map = std::vector<std::string>;
using Position = std::pair<int, int>;
using Decimal = float;

std::string getKey(const Map map);
std::string getKey(const Position manPos, const std::vector<Position> boxPos);
std::pair< Position, std::vector<Position> > getPositions(const Map map);
Map move(const Map& map, const MoveDirection direction, const Map& level);
void write_solution(const std::string filename, const Map& map, const std::vector<MoveDirection>& policy);
std::vector< std::pair<Map, std::vector<MoveDirection>> > read_solutions(std::string filename);
#endif