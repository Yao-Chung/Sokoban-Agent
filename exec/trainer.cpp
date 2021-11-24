#include <iostream>
#include <vector>
#include <filesystem>
#include <time.h>

#include <defines.hpp>
#include <Trainer.hpp>

#define PICK_NUM 5

int main(int argc, char const *argv[])
{
    // Parse command-line arguments
    if(argc < 3){
        std::cerr << "Usage: " << argv[0] << " <solution_file> <model_file> [--stream]" << std::endl;
        return -1;
    }
    std::string solution_path(argv[1]);
    Trainer trainer(argv[2]);
    // Train with multi maps
    if((argc > 3) && (std::string(argv[3]) == "--stream")){
        srand(time(NULL));
        // List all files in solution directory
        std::vector<std::string> allFiles;
        for(const auto & entry : std::filesystem::directory_iterator(solution_path)){
            allFiles.push_back(entry.path().string());
        }
        while(true){
            // Pick maps in files
            for(int i=1; i<=PICK_NUM; ++i){
                // Randomly select the file index
                int index = rand() % allFiles.size();
                int flag = 1;
                std::cerr << "===== " << i << "th: training with file " << allFiles[index] << " =====" << std::endl;
                for(auto [map, policy]: read_solutions(allFiles[index])){
                    std:: cerr << "====== accuracy of "  << flag << "th policy ======" << std::endl;
                    trainer.train(map, policy);
                    std:: cerr << "finish " << flag << "th policy" << std::endl;
                }
                trainer.save();
            }
        }
    }else{
        // Single file mode
        for(auto [map, policy]: read_solutions(argv[1])){
            trainer.train(map, policy);
        }
    }
    return 0;
}
