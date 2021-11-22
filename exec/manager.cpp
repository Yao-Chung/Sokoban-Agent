#include <iostream>
#include <defines.hpp>
#include <vector>
#include <ctime>
#include <fstream>

Map random_map(std::ifstream &fp, std::vector<int> startPos){
    int total = startPos.size();
    int index = rand() % total;
    fp.seekg(startPos[index], std::ios_base::beg);
    int32_t rows, cols;
    fp.read((char*)&rows, sizeof(int32_t));
    fp.read((char*)&cols, sizeof(int32_t));
    Map map(rows);
    for(std::string &row: map){
        row.resize(cols);
        fp.read(row.data(), cols);
    }
    return map;
}
std::vector<int> get_start_pos(std::ifstream &fp){
    std::vector<int> startPos;
    while(!fp.eof()){
        startPos.push_back((int)fp.tellg());
        int32_t rows, cols;
        fp.read((char*)&rows, sizeof(int32_t));
        fp.read((char*)&cols, sizeof(int32_t));
        fp.seekg(rows*cols, std::ios_base::cur);
        fp.peek();
    }
    return startPos;
}
int main(int argc, char *argv[]){
    srand (time(NULL));
    std::string filename = "ourMap.txt";
    std::ifstream fp(filename);
    std::vector<int> startPos = get_start_pos(fp);
    Map map = random_map(fp, startPos);
    return 0;
}