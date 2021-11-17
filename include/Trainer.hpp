#ifndef SOKOBAN_AGENT_TRAINER
#define SOKOBAN_AGENT_TRAINER

#include <utility>
#include <vector>
#include <defines.hpp>
#include <torch/torch.h>

struct Net: torch::nn::Module{
    Net();
    torch::Tensor forward(torch::Tensor input);
    torch::nn::Conv2d conv1, conv2;
    torch::nn::Linear fc1, fc2;
};

class Trainer{
public:
    Trainer(std::string filename);
    std::vector<Decimal> suggest(const Map map);
    void train(Map map, std::vector<MoveDirection> policy);
    void save();
private:
    torch::Tensor extract(const Map map);
    Net net;
    const std::string filename;
};

#endif