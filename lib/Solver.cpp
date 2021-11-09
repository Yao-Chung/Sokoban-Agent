#include <Solver.hpp>

#include <string>
#include <vector>

Solver::Solver(const Map map):
    map(map)
{ 
}

Map Solver::getMap(){
    return map;
}

Map Solver::move(const Map& map, const MoveDirection direction){
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
    result[manY][manX] = ((Solver::map[manY][manX] == '.') || (Solver::map[manY][manX] == '%')) ? '.' : ' ';
    return result;
}

bool Solver::isWin(const Map& map){
    for(std::string row: map){
        if(row.find('$') != std::string::npos){
            return false;
        }
    }
    return true;
}