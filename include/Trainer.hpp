#ifndef SOKOBAN_AGENT_TRAINER
#define SOKOBAN_AGENT_TRAINER

#include <utility>
#include <vector>
#include <defines.hpp>
#include <torch/torch.h>

class Trainer{
public:
    Trainer(/*TODO: weights*/);
    ~Trainer();
    std::vector<Decimal> suggest(const Map map);
    void train(std::vector< std::pair<Map, MoveDirection> > steps);
private:
    torch::Tensor extract(const Map map);
};

#endif