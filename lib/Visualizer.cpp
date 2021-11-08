#include <Visualizer.hpp>

#include <sstream>

Visualizer::Visualizer(std::string prefix, std::string extension):
    page(0), prefix(prefix), extension(extension)
{
    std::stringstream stream;
    stream << prefix <<"_" << page << extension;
    fout.open(stream.str());
}
Visualizer::~Visualizer(){
    fout.close();
}

void Visualizer::print(std::string data){
    fout << data;
}
void Visualizer::next(){
    std::stringstream stream(prefix);
    page += 1;
    fout.close();
    stream << prefix << "_" << page  << extension;
    fout.open(stream.str());
}