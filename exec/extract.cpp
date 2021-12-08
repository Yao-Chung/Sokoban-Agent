#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

#include <defines.hpp>

int main(int argc, char* const argv[]){
    if(argc != 4){
        std::cerr << "Usage: " << argv[0] << " <text_map_file> <binary_map_file> <index_file>" << std::endl;
        return -1;
    }
    std::ifstream text_map_file(argv[1]);
    std::ofstream binary_map_file(argv[2], std::ios::binary | std::ios::app);
    std::ofstream index_file(argv[3], std::ios::app);
    std::vector<std::string> map;
    int32_t cols = 0;
    // Read raw text file line by line
    for(std::string line; std::getline(text_map_file, line);){
        if(line.back() == '\r'){
            line.pop_back();
        }
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
    binary_map_file.write((char*)&rows, sizeof(int32_t));
    binary_map_file.write((char*)&cols, sizeof(int32_t));

    // Fill vertical
    for(int c = 0; c < cols; ++c){
        // Fill top
        for(int r = 0; r < rows; ++r){
            if(map[r].size() > c){
                if(map[r][c] != '#'){
                    map[r][c] = '#';
                }else{
                    break;
                }
            }
        }
        // Fill bottom
        for(int r = (rows - 1); r >= 0; --r){
            if(map[r].size() > c){
                if(map[r][c] != '#'){
                    map[r][c] = '#';
                }else{
                    break;
                }
            }
        }
    }
    // Fill horizontal & write
    for(std::string &row: map){
        for(int i = 0; i < row.size(); ++i){
            if(row[i] == '#'){
                break;
            }
            row[i] = '#';
        }
        row += std::string(cols - row.size(), '#');
        binary_map_file.write(row.c_str(), sizeof(char) * row.size());
    }
    
    // Write index
    index_file << argv[1] << " : " << std::hex << std::hash<std::string>{}(getKey(map)) << std::endl;
    // Clean
    binary_map_file.close();
    index_file.close();
    return 0;
}