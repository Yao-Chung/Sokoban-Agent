#include <Trainer.hpp>

#include <vector>

Trainer::Trainer(/*TODO: weights*/){

}

Trainer::~Trainer(){

}

std::vector<Decimal> Trainer::suggest(const Map map){

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
        .reshape({4, 9, 9});
}