#include <Visualizer.hpp>

#include <sstream>

Visualizer::Visualizer(std::string prefix, std::string extension):
    page(0), prefix(prefix), extension(extension)
{
    std::stringstream stream;
    stream << prefix << "_" << page << extension;
    out.open(stream.str());
}
Visualizer::~Visualizer(){
    out.close();
}

void Visualizer::next(){
    std::stringstream stream(prefix);
    //page += 1;
    out.close();
    stream << prefix << "_" << page  << extension;
    out.open(stream.str());
}