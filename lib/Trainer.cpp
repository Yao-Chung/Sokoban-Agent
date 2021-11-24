#include <Trainer.hpp>

#include <vector>
#include <valarray>
#include <filesystem>
#include <iomanip>

Net::Net():
    conv1(torch::nn::Conv2dOptions(4, 8, 5).stride(1)),
    conv2(torch::nn::Conv2dOptions(8, 16, 3).stride(1)),
    fc1(torch::nn::LinearOptions(144, 64).bias(false)),
    fc2(torch::nn::LinearOptions(64, 4).bias(false))
{
    register_module("conv1", conv1);
    register_module("conv2", conv2);
    register_module("fc1", fc1);
    register_module("fc2", fc2);
}

torch::Tensor Net::forward(torch::Tensor input){
    input = torch::relu(conv1(input));
    input = torch::relu(conv2(input));
    input = torch::flatten(input);
    input = torch::relu(fc1(input));
    input = fc2(input);
    return torch::softmax(input, 0);
}

Trainer::Trainer(std::string filename):
    filename(filename)
{
    if(std::filesystem::exists(filename)){
        torch::serialize::InputArchive input;
        input.load_from(filename);
        net.load(input);
    }
}

std::vector<Decimal> Trainer::suggest(const Map map){
    std::vector<Decimal> result(4);
    torch::Tensor output = net.forward(extract(map));
    memcpy(result.data(), output.data_ptr<Decimal>(), sizeof(Decimal) * 4);
    return result;
}

bool Trainer::train(Map level, std::vector<MoveDirection> policy){
    // Add optimizer
    torch::optim::Adam optimizer = torch::optim::Adam(net.parameters(), torch::optim::AdamOptions(1e-3));
    // Training
    Decimal lastAcc = 0.0;
    Decimal lastLoss = 0.0;
    for(size_t iter = 0, stuck = 0; stuck < stuck_limit; ++iter){
        Decimal accuracy = 0.0;
        Decimal avgLoss = 0.0;
        for(size_t epo = 0; epo < epoch; ++epo){
            int hitCount = 0;
            // Epoch
            Map map = level;
            Decimal sumLoss = 0.0;
            for(MoveDirection dir: policy){
                std::vector<Decimal> answerVec(4);
                answerVec[dir] = 1.0;
                torch::Tensor answer = torch::tensor(answerVec).reshape({1, 4});
                torch::Tensor predict = net.forward(extract(map)).reshape({1, 4});
                // Calculate loss
                torch::Tensor loss = torch::nn::functional::cross_entropy(answer, predict);
                sumLoss += *loss.data_ptr<Decimal>();
                // update parameters
                loss.backward();
                optimizer.step();
                optimizer.zero_grad();
                // Move to another state
                map = move(map, dir, level);
            }
            // Validate
            map = level;
            for(MoveDirection dir: policy){
                Decimal predicted = net.forward(extract(map)).data_ptr<Decimal>()[dir];
                if(predicted >= 0.25){
                    hitCount += 1;
                }
                map = move(map, dir, level);
            }
            // std::cout << "Hit: " << (Decimal) hitCount / (Decimal) policy.size() << std::endl;
            accuracy += (Decimal) hitCount / (Decimal) policy.size();
            avgLoss += (Decimal) sumLoss / (Decimal) policy.size();
        }
        accuracy /= (Decimal) epoch;
        avgLoss /= (Decimal) epoch;
        std::cout << "[" << iter << ":" << stuck << "] Accuracy: " << std::setprecision(6) << accuracy << ", Loss average: " << std::setprecision(20) << avgLoss  << std::endl;
        if(accuracy > threshold){
            return true;
        }else{
            if((std::abs(lastAcc - accuracy) < acc_thresh) && (std::abs(lastLoss - avgLoss) < loss_thresh)){
                stuck += 1;
            }else{
                stuck = 0;
            }
            lastLoss = avgLoss;
            lastAcc = accuracy;
        }
    }
    return false;
}

torch::Tensor Trainer::extract(const Map map){
    // Extract 9x9x5 from given map (#, ., $, %)
    union{
        Decimal input3D[4][9][9];
        Decimal input1D[324];
    } data;
    memset(data.input1D, 0, sizeof(Decimal) * 324);
    int rows = map.size(), cols = map[0].size();
    // Get man position
    int manRow = -1, manCol = -1;
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            if(map[i][j] == '@'){
                manRow = i, manCol = j;
            }
        }
    }
    // Get items position
    for(int i=-4; i<=4; i++){
        for(int j=-4; j<=4; j++){
            int nextRow = manRow + i, nextCol = manCol + j;
            if(nextRow < 0 || nextRow >= rows || nextCol < 0 
            || nextCol >= cols || map[nextRow][nextCol] == '#'){
                // Wall
                data.input3D[0][i+4][j+4] = 1.0;
            }else{
                switch(map[nextRow][nextCol]){
                    case '.':
                        data.input3D[1][i+4][j+4] = 1.0;
                        break;
                    case '$':
                        data.input3D[2][i+4][j+4] = 1.0;
                        break;
                    case '%':
                        data.input3D[3][i+4][j+4] = 1.0;
                        break;
                }
            }
        }
    }
    // Convert to tensor
    return torch::tensor(
        std::vector<Decimal>(std::begin(data.input1D), std::end(data.input1D)))
        .reshape({1, 4, 9, 9});
}

void Trainer::save(){
    torch::serialize::OutputArchive output;
    net.save(output);
    output.save_to(filename);
}
