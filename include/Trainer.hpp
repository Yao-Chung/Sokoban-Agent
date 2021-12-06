#ifndef SOKOBAN_AGENT_TRAINER
#define SOKOBAN_AGENT_TRAINER

#include <utility>
#include <vector>
#include <defines.hpp>
#include <torch/torch.h>

#define stuck_limit 20
#define acc_thresh 1e-6
#define epoch 30
#define threshold 0.7

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
    bool train(Map level, std::vector<MoveDirection> policy); // true if accuracy > threshold
    void reload();
    void save();
private:
    torch::Device device;
    torch::Tensor extract(const Map map);
    Net net;
    const std::string filename;
};

#endif