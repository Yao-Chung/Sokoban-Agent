#include <string>
#include <vector>
#include <iostream>

#include <HttpClient.hpp>
#include <Solver.hpp>
#include <PolicyOptimize.hpp>

static void printMap(const Map& map){
    for(std::string row : map){
        std::cout << row << std::endl;
    }
}

int main(int argc, char* const argv[])
{
    std::string prefix, extension;
    if(argc > 1){
        prefix = argv[1];
        extension = ".dot";
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
    std::vector<std::string> &level = levels[0];

    // Create solver
    Solver solver(level, prefix, extension);
    // Solve
    std::vector<MoveDirection> policy = solver.solve();
    std:: cout << "=== Finished ===" << std::endl;
    std:: cout << "policy size = " << policy.size() << std::endl;
    // Write solution to file
    write_solution("solution1.txt", level, policy);
    
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
