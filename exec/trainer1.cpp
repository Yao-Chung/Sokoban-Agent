#include <iostream>
#include <vector>
#include <defines.hpp>
#include <Trainer.hpp>

int main(int argc, char const *argv[])
{
    // Parse command-line arguments
    if(argc != 3){
        std::cerr << "Usage: " << argv[0] << " <solution_file> <model_file>" << std::endl;
        return -1;
    }
    // Read solution
    std::vector< std::pair<Map, std::vector<MoveDirection>> > solutions = read_solutions(argv[1]);
    // Create Trainer
    Trainer trainer(argv[2]);
    // Give suggestion
    std::vector<Decimal> suggest = trainer.suggest(solutions[1].first);
    // Save model
    trainer.save();
    return 0;
}
