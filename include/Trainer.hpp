#ifndef SOKOBAN_AGENT_TRAINER
#define SOKOBAN_AGENT_TRAINER

#include <utility>
#include <vector>
#include <defines.hpp>

class Trainer{
    Trainer(/*TODO: weights*/);
    ~Trainer();
    std::vector<Decimal> suggest(const Map map);
    void train(std::vector< std::pair<Map, MoveDirection> > steps);
};

#endif