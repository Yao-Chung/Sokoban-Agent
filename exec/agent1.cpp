#include <string>
#include <vector>
#include <iostream>

#include <HttpClient.hpp>
#include <Solver.hpp>

static void printMap(const Map& map){
    for(std::string row : map){
        std::cout << row << std::endl;
    }
}

int main(int argc, char* const argv[])
{
    std::string prefix, extension, netPath, solutionPath;
    size_t mapId = 0;
    if(argc < 4){
        std::cerr << "Usage: " << argv[0] << " <net_file> <solution_file> <mapId>" << std::endl;
    }else{
        netPath = argv[1];
        solutionPath = argv[2];
        mapId = std::atoi(argv[3]);
        if(argc > 4){
            prefix = argv[4];
            extension = ".dot";
        }
    }
    

    // Get map from server
    // HttpClient client("sokoban.luishsu.me", "80");
    // std::vector<std::string> map = client.start("sokoban01.txt");
    // for(std::string row : map){
    //     std::cout << row << std::endl;
    // }
    
    // FIXME: Hard-coded map
    std::vector<std::vector<std::string>> levels = {
        {
            // Teacher
            "########",
            "#. #   #",
            "#  $   #",
            "#   # ##",
            "## # $.#",
            "#   $  #",
            "#  .# @#",
            "########",
        },
        {
            // Medium
            "##########",
            "# ###### #",
            "# #####. #",
            "# #####  #",
            "#  ### $ #",
            "#@$    . #",
            "# .# $.  #",
            "# $####  #",
            "#    #####",
            "##########",
        },
        {
            // Medium
            "##########",
            "# ###    #",
            "# @## $. #",
            "# $. .$$ #",
            "#   # .  #",
            "##########",
        },
        {
            // Medium
            "##########",
            "###@######",
            "# .$### ##",
            "# $  . $ #",
            "#    . $ #",
            "## ## #  #",
            "#####  # #",
            "#### .#  #",
            "####     #",
            "##########",
        },
        {
            // Hard
            "#######",
            "##### #",
            "#@ ## #",
            "# $   #",
            "## $  #",
            "#  #..#",
            "# $$  #",
            "# .  .#",
            "#######",
        },
        {
            // Hard
            "##########",
            "######   #",
            "#######$ #",
            "####### .#",
            "#  ### $ #",
            "#.$ #    #",
            "#  @#  ###",
            "# $ ##  ##",
            "#  ..    #",
            "##########",
        },
        {
            // Hard
            "##########",
            "#        #",
            "# .$ #   #",
            "###$.##  #",
            "###  # # #",
            "## . $.  #",
            "####   # #",
            "####$#####",
            "### @    #",
            "##########",
        },
        {
            // Hard
            "##########",
            "## #######",
            "##.#######",
            "#.$.######",
            "#   ######",
            "# $#######",
            "#  #######",
            "# $#######",
            "#@$.     #",
            "##########",
        },
        {
            // Hard
            "##########",
            "#  #######",
            "# $ ######",
            "#. #######",
            "# . ######",
            "#   $. @##",
            "#  #  ####",
            "# ## $ ###",
            "# $ .    #",
            "##########",
        }
    };
    std::vector<std::string> &level = levels[mapId];

    // Create solver
    Solver solver(level, netPath, prefix, extension);
    // Solve
    std::vector<MoveDirection> policy = solver.solve();
    std:: cout << "=== Finished ===" << std::endl;
    std:: cout << "policy size = " << policy.size() << std::endl;
    // Write solution to file
    write_solution(solutionPath, level, policy);
    
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
