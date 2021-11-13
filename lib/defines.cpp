#include <defines.hpp>

#include <sstream>

std::string getKey(const Map map){
    std::pair< Position, std::vector<Position> > positions = getPositions(map);
    return getKey(positions.first, positions.second);
}

std::string getKey(const Position manPos, const std::vector<Position> boxPos){
    std::stringstream stream;
    stream << "M:(" << manPos.first << "," << manPos.second << ")";
    stream << "B:";
    for(Position pos: boxPos){
        stream << "(" << pos.first << "," << pos.second << ")";
    }
    return stream.str();
}

std::pair< Position, std::vector<Position> > getPositions(const Map map){
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

Map move(const Map& map, const MoveDirection direction, const Map& level){
    Map result(map);
    // Find man position
    size_t manX = std::string::npos, manY = std::string::npos;
    for(size_t y = 0; y < map.size(); ++y){
        manX = map.at(y).find('@');
        if(manX != std::string::npos){
            manY = y;
            break;
        }
    }
    if((manX == std::string::npos) && (manY = std::string::npos)){
        // TODO: error handling
    }
    // Check along direction
    int fwdX = manX, fwdY = manY;
    int ffwdX = manX, ffwdY = manY;
    switch(direction){
        case MoveDirection::Up:
            fwdY -= 1;
            ffwdY -= 2;
        break;
        case MoveDirection::Down:
            fwdY += 1;
            ffwdY += 2;
        break;
        case MoveDirection::Left:
            fwdX -= 1;
            ffwdX -= 2;
        break;
        case MoveDirection::Right:
            fwdX += 1;
            ffwdX += 2;
        break;
        default:
        break;
    }
    // Out of bound
    if((fwdX < 0) || (fwdY < 0) || (fwdX >= map[0].size()) || (fwdY >= map.size())){
        return result;
    }
    // Hit wall
    switch(map[fwdY][fwdX]){
        case '#': 
            // Wall
            return result;
        case '$':
        case '%':
            // Box
            if(
                (ffwdX < 0) || (ffwdY < 0) || (ffwdX >= map[0].size()) || (ffwdY >= map.size())
                || ((map[ffwdY][ffwdX] != ' ') && (map[ffwdY][ffwdX] != '.'))
            ){
                // Can't move
                return result;
            }else {
                // Move box
                result[ffwdY][ffwdX] = (map[ffwdY][ffwdX] == '.') ? '%' : '$';
            }
        break;
        default:
        break;
    }
    // Move man
    result[fwdY][fwdX] = '@';
    result[manY][manX] = ((level[manY][manX] == '.') || (level[manY][manX] == '%')) ? '.' : ' ';
    return result;
}