#include <defines.hpp>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

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

std::vector< std::pair<Map, std::vector<MoveDirection>> > read_solutions(std::string filename){
    // Open file in binary mode
    std::ifstream fin(filename, std::ios::binary);
    // Read solutions
    std::vector< std::pair<Map, std::vector<MoveDirection>> > solutions;
    while(!fin.eof()){
        int32_t rows, cols;
        // Read row & column
        fin.read((char*)&rows, sizeof(int32_t));
        fin.read((char*)&cols, sizeof(int32_t));
        // Read map
        Map map(rows);
        for(int32_t i = 0; i < rows; ++i){
            map[i].resize(cols);
            fin.read(map[i].data(), cols);
        }
        // Read policy size
        int32_t pSize;
        fin.read((char*)&pSize, sizeof(int32_t));
        // Read policy
        std::vector<MoveDirection> policy(pSize);
        fin.read((char*)policy.data(), sizeof(MoveDirection) * pSize);
        // Push to solution
        solutions.emplace_back(map, policy);
        fin.peek();
    }
    // Close file
    fin.close();
    return solutions;
}

void write_solution(const std::string filename, const Map& map, const std::vector<MoveDirection> &policy){
    // Open the file
    std::FILE* fp = std::fopen(filename.c_str(), "ab");
    // Write row and column length to file
    int32_t rows = map.size();
    int32_t cols = map[0].size();
    fwrite(&rows, sizeof(int32_t), 1, fp);
    fwrite(&cols, sizeof(int32_t), 1, fp);
    // Write map to file
    for(std::string row: map){
        fwrite(row.c_str(), sizeof(char), row.size(), fp);
    }
    // Write policy size to file
    int32_t policy_size = policy.size();
    fwrite(&policy_size, sizeof(policy_size), 1, fp);
    // Write policy to file
    fwrite(policy.data(), sizeof(MoveDirection), policy.size(), fp);
    fclose(fp);
}

Map readMap(std::string filename){
    std::ifstream fin(filename, std::ios::binary);
    int32_t size[2];
    fin.read((char*)size, sizeof(int32_t) * 2);
    Map map(size[0]);
    for(std::string &row: map){
        row.resize(size[1]);
        fin.read(row.data(), size[1]);
    }
    fin.close();
    return map;
}

std::vector< std::pair<Map, std::vector<MoveDirection>> > clean_solutions(std::vector< std::pair<Map, std::vector<MoveDirection>> > solutions){
    // Create a unordered_map for unique solutions
    std::unordered_map< std::string, std::pair<Map, std::unordered_set<std::string> > > map_to_solutions;
    for(auto solution: solutions){
        // transform vector<MoveDirection> to string
        std::string steps("");
        for(MoveDirection dir: solution.second){
            steps += (dir+'0');
        }
        // Put steps into map_to_solutions
        map_to_solutions[getKey(solution.first)].first = solution.first;
        map_to_solutions[getKey(solution.first)].second.insert(steps);
    }
    std::vector< std::pair<Map, std::vector<MoveDirection>> > unique_solutions;
    for(auto [key, p]: map_to_solutions){
        for(std::string steps: p.second){
            // transform string to vector<MoveDirection>
            std::vector<MoveDirection> solution;
            for(auto c: steps){
                solution.emplace_back(c-'0');
            }
            unique_solutions.push_back({p.first, solution});
        }
    }
    return unique_solutions;
}