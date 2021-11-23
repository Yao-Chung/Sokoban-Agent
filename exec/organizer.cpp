#include <iostream>
#include <vector>
#include <unordered_set>
#include <utility>
#include <string>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <defines.hpp>

int main(int argc, char *argv[]){
    // Print usage
    if(argc < 4){
        std::cerr << "Usage: " << argv[0] << " <solutions_dir> <output_file> [input_files]+" << std::endl;
        return -1;
    }
    // Ensure folder exists
    std::filesystem::path directory(argv[1]);
    if(std::filesystem::exists(directory)){
        if(!std::filesystem::is_directory(directory)){
            std::cerr << "'"<< directory << "' is not a directory" << std::endl;
            return -1;
        }
    }else{
        std::filesystem::create_directories(directory);
    }
    // Read solutions & write to map file
    std::vector< std::pair<Map, std::vector<MoveDirection>> > solutions;
    std::unordered_set<std::string> mapKeys;
    for(int i = 3; i < argc; ++i){
        for(auto [map, policy]: read_solutions(argv[i])){
            std::stringstream stream;
            stream << std::hex << std::hash<std::string>{}(getKey(map));
            std::string key = stream.str();
            mapKeys.insert(key);
            write_solution(std::filesystem::path(directory).append(key).string(), map, policy);
            solutions.emplace_back(map, policy);
        }
    }
    // Clean solutions & write to file
    for(auto [map, policy]: clean_solutions(solutions)){
        write_solution(argv[2], map, policy);
    }
    // Read map solutions, clean & write
    for(std::string key: mapKeys){
        std::filesystem::path filepath(directory);
        filepath /= key;
        solutions = clean_solutions(read_solutions(filepath.string()));
        std::ofstream cleanOut(filepath, std::ios::trunc);
        cleanOut.close();
        for(auto [map, policy]: solutions){
            write_solution(filepath.string(), map, policy);
        }
    }
    return 0;
}