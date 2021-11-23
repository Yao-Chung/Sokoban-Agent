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
    std::string prefix, extension, netPath, solutionPath;
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <solution_file> [net_file] [dot_file_prefix]" << std::endl;
        return -1;
    }else{
        solutionPath = argv[1];
        if(argc > 2){
            netPath = argv[2];
        }
        if(argc > 3){
            prefix = argv[3];
            extension = ".dot";
        }
    }
    
    Map level = readMap(std::cin);
    printMap(level);

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
