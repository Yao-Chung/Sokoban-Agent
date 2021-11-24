#include <iostream>
#include <vector>
#include <filesystem>
#include <time.h>
#include <fstream>

#include <defines.hpp>
#include <Trainer.hpp>

#define PICK_NUM 5

int main(int argc, char const *argv[])
{
    // Parse command-line arguments
    if(argc < 3){
        std::cerr << "Usage: " << argv[0] << " <solution_file> <model_file> [--stream] [<hard_record_file>]" << std::endl;
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
            if(entry.path().string()[0] != '.'){
                allFiles.push_back(entry.path().string());
            }
        }
        while(true){
            // Pick maps in files
            for(int i = 1; i <= PICK_NUM; ++i){
                // Randomly select the file index
                int index = rand() % allFiles.size();
                int flag = 1;
                std::cerr << "===== " << i << "th: training with file " << allFiles[index] << " =====" << std::endl;
                for(auto [map, policy]: read_solutions(allFiles[index])){
                    std:: cerr << "====== accuracy of "  << flag << "th solution ======" << std::endl;
                    if(trainer.train(map, policy)){
                        trainer.save();
                    }else{
                        std::cerr << "It's hard" << std::endl;
                        if(argc > 4){
                            std::ofstream hardOut(argv[4], std::ios::app);
                            hardOut << allFiles[index] << std::endl;
                            hardOut.close();
                        }
                        trainer.reload();
                    }
                    std:: cerr << "finish " << flag << "th solution" << std::endl;
                    flag += 1;
                }
            }
        }
    }else{
        // Single file mode
        for(auto [map, policy]: read_solutions(argv[1])){
            if(!trainer.train(map, policy)){
                std::cerr << "It's hard" << std::endl;
            }
        }
        trainer.save();
    }
    return 0;
}
