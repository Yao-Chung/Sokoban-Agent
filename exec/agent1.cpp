#include <string>
#include <vector>
#include <iostream>
#include <any>
#include <cstdlib>
#include <unordered_map>
#include <HttpClient.hpp>

#include <Solver.hpp>
#include <PolicyOptimize.hpp>

static void printMap(const Map& map){
    for(std::string row : map){
        std::cout << row << std::endl;
    }
}

int main(void)
{
    // Get map from server
    // HttpClient client("sokoban.luishsu.me", "80");
    // std::vector<std::string> map = client.start("sokoban01.txt");
    // for(std::string row : map){
    //     std::cout << row << std::endl;
    // }
    
    // FIXME: Hard-coded map
    std::vector<std::string> level = {
        "########",
        "#. #   #",
        "#  $   #",
        "#   # ##",
        "## # $.#",
        "#   $  #",
        "#  .# @#",
        "########",
    };

    // Create solver
    Solver solver(level);
    // Solve
    std::vector<MoveDirection> policy = solver.solve();
    // Optimize policy
    policy = PolicyOptimize::optimize(level, policy);
    // Replay policy
    std::cout << "== Replay ==" << std::endl;
    std::vector<std::string> map(level);
    for(MoveDirection direction: policy){
        map = move(map, direction, level);
        switch(direction){
            case MoveDirection::Up:
                std::cout << "Up" << std::endl;
            break;
            case MoveDirection::Down:
                std::cout << "Down" << std::endl;
            break;
            case MoveDirection::Left:
                std::cout << "Left" << std::endl;
            break;
            case MoveDirection::Right:
                std::cout << "Right" << std::endl;
            break;
            default:
            break;
        }
        printMap(map);
    }
    return 0;
}
