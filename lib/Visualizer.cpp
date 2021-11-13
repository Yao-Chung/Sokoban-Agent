#include <Visualizer.hpp>

#include <sstream>

Visualizer::Visualizer(std::string prefix, std::string extension):
    page(0), prefix(prefix), extension(extension)
{
}
Visualizer::~Visualizer(){
    out.close();
}

void Visualizer::next(size_t advance){
    if(out.is_open()){
        out.close();
    }
    page += advance;
    std::stringstream stream(prefix);
    stream << prefix << "_" << page << extension;
    out.open(stream.str());
}