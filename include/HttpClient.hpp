#ifndef SOKOBAN_AGENT_HTTPCLIENT
#define SOKOBAN_AGENT_HTTPCLIENT

#include <string>
#include <vector>
#include <utility>
#include <netdb.h>

class HttpClient{
    private:
        struct addrinfo *servinfo;
        std::string cookie;
    public:
        HttpClient(std::string hostname, std::string port);
        std::string send_request(std::string url);
        std::pair<bool, std::vector<std::string>> move(int direction);
        std::vector<std::string> restart();
        std::vector<std::string> start(std::string level);
};

#endif