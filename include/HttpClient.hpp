#ifndef SOKOBAN_AGENT_HTTPCLIENT
#define SOKOBAN_AGENT_HTTPCLIENT

#include <string>
#include <vector>
#include <utility>
#include <netdb.h>
#include <defines.hpp>

class HttpClient{
    private:
        struct addrinfo *servinfo;
        std::string cookie;
        std::string send_request(std::string url);
    public:
        HttpClient(std::string hostname, std::string port);
        ~HttpClient();
        std::pair<bool, Map> move(MoveDirection direction);
        Map restart();
        Map start(std::string level);
};

#endif