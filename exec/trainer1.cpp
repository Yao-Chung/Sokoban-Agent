#include <iostream>
#include <vector>
#include <defines.hpp>

static void printMap(const Map& map){
    for(std::string row : map){
        std::cout << row << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    // Parse command-line arguments
    if(argc != 2){
        std::cerr << "Usage: " << argv[0] << " <solution_file>" << std::endl;
        return -1;
    }
    // Read solution
    std::vector< std::pair<Map, std::vector<MoveDirection>> > solutions = read_solutions(argv[1]);
    // Print solution
    for(auto [map, policy]: solutions){
        printMap(map);
        std::cout << std::endl;
        for(MoveDirection direction: policy){
            switch (direction){
            case MoveDirection::Up :
                std::cout << "Up,";
                break;
            case MoveDirection::Down :
                std::cout << "Down,";
                break;
            case MoveDirection::Left :
                std::cout << "Left,";
                break;
            case MoveDirection::Right :
                std::cout << "Right,";
                break;
            default:
                break;
            }
        }
        std::cout << std::endl;
    }
    return 0;
}
