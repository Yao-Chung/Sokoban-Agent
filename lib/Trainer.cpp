#include <Trainer.hpp>

#include <vector>
#include <filesystem>

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

void Trainer::train(std::vector< std::pair<Map, MoveDirection> > steps){

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
