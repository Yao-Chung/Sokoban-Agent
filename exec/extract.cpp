#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char* const argv[]){
    if(argc != 3){
        std::cerr << "Usage: " << argv[0] << " <deepmind_file> <our_file>" << std::endl;
        return -1;
    }
    std::string deepmindFile = argv[1];
    std::string ourFile = argv[2];
    std::ifstream inputFile(deepmindFile);
    std::FILE* fp = std::fopen(ourFile.c_str(), "ab");
    std::string line;
    std::vector<std::string> map;
    while(std::getline(inputFile, line)){
        // Read map
        if(!line.empty() && line[0] != ';'){
            map.push_back(line);
        }
        // Read "\n" or "; id"
        else if(!map.empty()){
            int32_t rows = map.size();
            int32_t cols = map[0].size();
            fwrite(&rows, sizeof(int32_t), 1, fp);
            fwrite(&cols, sizeof(int32_t), 1, fp);
            for(auto s: map){
                fwrite(s.c_str(), sizeof(char), s.size(), fp);
            }
            map.clear();
        }
    }
    return 0;
}