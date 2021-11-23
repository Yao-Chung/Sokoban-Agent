#include <iostream>
#include <vector>
#include <defines.hpp>
#include <Trainer.hpp>

int main(int argc, char const *argv[])
{
    // Parse command-line arguments
    if(argc < 3){
        std::cerr << "Usage: " << argv[0] << " <solution_file> <model_file> [--stream]" << std::endl;
        return -1;
    }
    Trainer trainer(argv[2]);
    if((argc > 3) && (std::string(argv[3]) == "--stream")){
        // Stream mode
        // TODO:
    }else{
        // Single file mode
        std::vector< std::pair<Map, std::vector<MoveDirection>> > solutions = read_solutions(argv[1]);
        trainer.train(solutions[0].first, solutions[0].second);
    }
    trainer.save();
    return 0;
}
