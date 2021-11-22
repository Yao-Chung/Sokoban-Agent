#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include <defines.hpp>

int main(int argc, char *argv[]){
    std::string input = "repeat.txt";
    std::string output = "unique.txt";
    std::vector< std::pair<Map, std::vector<MoveDirection>> > all_solutions = read_solutions(input);
    std::vector< std::pair<Map, std::vector<MoveDirection>> > unique_solutions = clean_solutions(all_solutions);
    for(auto p: unique_solutions){
        write_solution(output, p.first, p.second);
    }
    return 0;
}