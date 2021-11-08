#ifndef SOKOBAN_AGENT_MARKLOVSOLVER_VISUALIZER
#define SOKOBAN_AGENT_MARKLOVSOLVER_VISUALIZER

#include <string>
#include <fstream>

class Visualizer{
public:
    Visualizer(std::string prefix, std::string extension);
    ~Visualizer();
    std::ofstream out;
    void next();
private:
    size_t page;
    std::string prefix, extension;
};

#endif