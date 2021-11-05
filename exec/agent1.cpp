#include <HttpClient.hpp>
#include <string>
#include <vector>
#include <iostream>

int main(int argc, char const *argv[])
{
    HttpClient client("sokoban.luishsu.me", "80");
    std::vector<std::string> map = client.start("sokoban00.txt");
    for(std::string row : map){
        std::cout << row << std::endl;
    }
    auto moveResult = client.move(MoveDirection::Down);
    client.restart();
    for(std::string row : map){
        std::cout << row << std::endl;
    }
    return 0;
}
