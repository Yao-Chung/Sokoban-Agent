#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <defines.hpp>
#include <Solver.hpp>

static void printMap(const Map& map){
    for(std::string row : map){
        std::cout << row << std::endl;
    }
}

int main(int argc, char* const argv[])
{
    std::string prefix, extension, mapPath, netPath, solutionPath;
    if(argc < 3){
        std::cerr << "Usage: " << argv[0] << " <map_file> <solution_file> [net_file] [dot_file_prefix]" << std::endl;
        return -1;
    }else{
        mapPath = argv[1];
        solutionPath = argv[2];
        if(argc > 3){
            netPath = argv[3];
        }
        if(argc > 4){
            prefix = argv[4];
            extension = ".dot";
        }
    }
    std::ifstream fin(mapPath, std::ios::binary);
    Map level = readMap(fin);
    fin.close();

    // Create solver
    Solver solver(level, netPath, prefix, extension);
    // Solve
    std::vector<MoveDirection> policy = solver.solve();
    std:: cout << "=== Finished ===" << std::endl;
    std:: cout << "total steps = " << policy.size() << std::endl;
    // Write solution to file
    write_solution(solutionPath, level, policy);
    
    // Replay policy
    std::cout << "== Solution ==" << std::endl;
    std::vector<std::string> map(level);
    for(MoveDirection direction: policy){
        map = move(map, direction, level);
        switch(direction){
            case MoveDirection::Up:
                std::cout << "Up, ";
            break;
            case MoveDirection::Down:
                std::cout << "Down, ";
            break;
            case MoveDirection::Left:
                std::cout << "Left, ";
            break;
            case MoveDirection::Right:
                std::cout << "Right, ";
            break;
            default:
            break;
        }
    }
    std::cout << std::endl;
    std::cout << "== Final state ==" << std::endl;
    printMap(map);
    return 0;
}
