#ifndef SOKOBAN_AGENT_MARKLOVSOLVER_VISUALIZER
#define SOKOBAN_AGENT_MARKLOVSOLVER_VISUALIZER

#include <string>
#include <fstream>

class Visualizer{
public:
    Visualizer(std::string prefix, std::string extension);
    ~Visualizer();
    void print(std::string data);
    void next();
private:
    size_t page;
    std::ofstream fout;
    std::string prefix, extension;
};

#endif