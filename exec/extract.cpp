#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char* const argv[]){
    if(argc != 3){
        std::cerr << "Usage: " << argv[0] << " <text_map_file> <binary_map_file>" << std::endl;
        return -1;
    }
    std::ifstream text_map_file(argv[1]);
    std::FILE* binary_map_file = std::fopen(argv[2], "ab");
    std::string line;
    std::vector<std::string> map;
    int32_t cols = 0;
    // Read raw text file line by line
    while(std::getline(text_map_file, line)){
        map.push_back(line);
        // Get the longest column number
        if(line.size() > cols){
            cols = line.size();
        }
    }
    text_map_file.close();
    // Get row number of map
    int32_t rows = map.size();
    // Write row number and col number to binary_file
    // fwrite(&rows, sizeof(int32_t), 1, fp);
    // fwrite(&cols, sizeof(int32_t), 1, fp);
    for(std::string &row: map){
        for(int i=0; i<row.size(); i++){
            if(row[i] != ' '){
                break;
            }
            row[i] = '#';
        }
        row += string(cols - row.size(), '#');
    }
    for(auto s: map){
        fwrite(s.c_str(), sizeof(char), s.size(), binary_map_file);
    }
    //     // Read "\n" or "; id"
    //     else if(!map.empty()){
    //         int32_t rows = map.size();
    //         int32_t cols = map[0].size();
    //         fwrite(&rows, sizeof(int32_t), 1, fp);
    //         fwrite(&cols, sizeof(int32_t), 1, fp);
    //         for(auto s: map){
    //             fwrite(s.c_str(), sizeof(char), s.size(), fp);
    //         }
    //         map.clear();
    //     }
    // }
    fclose(binary_map_file);
    return 0;
}