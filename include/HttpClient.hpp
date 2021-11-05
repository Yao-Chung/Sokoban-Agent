#ifndef SOKOBAN_AGENT_HTTPCLIENT
#define SOKOBAN_AGENT_HTTPCLIENT

#include <string>
#include <vector>
#include <utility>
#include <netdb.h>

enum class MoveDirection{
    Left,
    Right,
    Up,
    Down,
};

class HttpClient{
    private:
        struct addrinfo *servinfo;
        std::string cookie;
        std::string send_request(std::string url);
    public:
        HttpClient(std::string hostname, std::string port);
        ~HttpClient();
        std::pair< bool, std::vector<std::string> > move(MoveDirection direction);
        std::vector<std::string> restart();
        std::vector<std::string> start(std::string level);
};

#endif