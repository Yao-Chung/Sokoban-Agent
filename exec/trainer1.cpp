#include <iostream>
#include <vector>
#include <defines.hpp>
#include <Trainer.hpp>

int main(int argc, char const *argv[])
{
    // Parse command-line arguments
    if(argc != 2){
        std::cerr << "Usage: " << argv[0] << " <solution_file>" << std::endl;
        return -1;
    }
    // Read solution
    std::vector< std::pair<Map, std::vector<MoveDirection>> > solutions = read_solutions(argv[1]);
    // Create Trainer
    Trainer trainer;
    std::cout << trainer.extract(solutions[0].first) << std::endl;
    return 0;
}
