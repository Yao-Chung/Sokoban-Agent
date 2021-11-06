#include <MarklovSolver/State.hpp>

#include <sstream>

State::State(unsigned int distance, Map map){

}

std::string State::getKey(const Map map){
    std::pair< Position, std::vector<Position> > positions = getPositions(map);
    return getKey(positions.first, positions.second);
}

std::string State::getKey(const Position manPos, const std::vector<Position> boxPos){
    std::stringstream stream;
    stream << "M:(" << manPos.first << "," << manPos.second << ")";
    stream << "B:";
    for(Position pos: boxPos){
        stream << "(" << pos.first << "," << pos.second << ")";
    }
    return stream.str();
}

std::pair< Position, std::vector<Position> > State::getPositions(const Map map){
    Position manPos;
    std::vector<Position> boxPos;
    for(size_t y = 0; y < map.size(); ++y){
        for(size_t x = 0; x < map[y].size(); ++x){
            switch(map[y][x]){
                case '@':
                    manPos = {x, y};
                break;
                case '$':
                case '%':
                    boxPos.emplace_back(x, y);
                break;
            }
        }
    }
    return {manPos, boxPos};
}