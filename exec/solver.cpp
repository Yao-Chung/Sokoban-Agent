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
    size_t mapId = 0;
    if(argc < 4){
        std::cerr << "Usage: " << argv[0] << " <map_file> <solution_file> <net_file>" << std::endl;
        return -1;
    }else{
        mapPath = argv[1];
        solutionPath = argv[2];
        netPath = argv[3];
        if(argc > 4){
            prefix = argv[4];
            extension = ".dot";
        }
    }
    
    Map level = readMap(mapPath);

    // Create solver
    Solver solver(level, netPath, prefix, extension);
    // Solve
    std::vector<MoveDirection> policy = solver.solve();
    std:: cout << "=== Finished ===" << std::endl;
    std:: cout << "policy size = " << policy.size() << std::endl;
    // Write solution to file
    write_solution(solutionPath, level, policy);
    return 0;
}
