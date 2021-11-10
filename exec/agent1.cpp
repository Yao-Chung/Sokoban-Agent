#include <string>
#include <vector>
#include <iostream>
#include <any>
#include <cstdlib>
#include <unordered_map>
#include <HttpClient.hpp>

#include <MarklovSolver/MarklovSolver.hpp>
#include <MarklovSolver/PolicyVisualizer.hpp>
#include <PolicyOptimize.hpp>

static void printUsage(char const *argv0){
    std::cerr << "Usage: " << argv0
        << " <alpha_value>"
        << " <beta_value>"
        << " <gamma_value>"
        << " <iter_value>"
        << " <iter_delta_value>"
    << "\n" << std::endl;
}

static std::unordered_map<std::string, std::any> parseArgs(int argc, char const *argv[]){
    if(argc < 6){
        std::cerr << "Error: Insufficient command-line arguments!" << std::endl;
        printUsage(argv[0]);
        std::exit(-1);
    }
    std::unordered_map<std::string, std::any> result;
    result.emplace("alpha", (float)std::atof(argv[1]));
    result.emplace("beta", (float)std::atof(argv[2]));
    result.emplace("gamma", (float)std::atof(argv[3]));
    result.emplace("iter", (unsigned int)std::atoi(argv[4]));
    result.emplace("iter_delta", (unsigned int)std::atoi(argv[5]));
    return result;
}

static void printMap(const Map& map){
    for(std::string row : map){
        std::cout << row << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    // Parse command-line arguments
    std::unordered_map<std::string, std::any> args(parseArgs(argc, argv));
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

    // Marklov solver
    MarklovSolver solver(
        std::any_cast<float>(args["alpha"]),
        std::any_cast<float>(args["beta"]),
        std::any_cast<float>(args["gamma"]),
        std::any_cast<unsigned int>(args["iter"]),
        std::any_cast<unsigned int>(args["iter_delta"]),
        level
    );
    solver.attach_Visualizer("output/out", ".dot");
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
        }
        printMap(map);
    }
    // Print States
    PolicyVisualizer policyVisualizer("states", ".dot");
    policyVisualizer.visualize(policy, solver.rootState);
    return 0;
}
