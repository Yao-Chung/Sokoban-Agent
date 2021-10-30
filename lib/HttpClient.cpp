#include <HttpClient.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define MAXDATASIZE 1024

HttpClient::HttpClient(std::string hostname, std::string port){
    // getaddrinfo only needs to be executed once
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    // get the servinfo
    if(getaddrinfo(hostname.c_str(), port.c_str(), &hints, &servinfo) != 0)
    {
        //TODO: exception handle
    }
}
HttpClient::~HttpClient(){
    freeaddrinfo(servinfo);
}
std::string HttpClient::send_request(std::string url){
    // open socket
    int sockfd;
    if((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
    {
        //TODO: exception handle
    }
    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        //TODO: exception handle
    }
    // send GET request
    std::string msg("GET ");
    msg += (url + " HTTP/1.1\r\n\r\n");
    if (send(sockfd, msg.c_str(), msg.size(), 0) == -1) 
    {
        //TODO: exception handle
    }
    // receive message
    std::string recv_str(1024, '\0');
    std::string all_content("");
    ssize_t recv_size;
    size_t body_size = 0;
    while((recv_size = recv(sockfd, recv_str.data(), recv_str.size(), 0)) > 0){
        all_content += recv_str.substr(0, recv_size);
        // find the empty line
        size_t index;
        if((index = all_content.find("\r\n\r\n")) != std::string::npos){
            // Get content length
            std::string remain = all_content.substr(all_content.find("Content-length: ")+16);
            std::string content_length = remain.substr(0, remain.find("\r"));
            body_size = std::stol(content_length);
            all_content = all_content.substr(index+4);
            recv_str = std::string(1024, '\0');
            break;
        }
        recv_str = std::string(1024, '\0');
    }
    if(recv_size == -1){
        //TODO: exception handle
    }
    // Get the whole body
    while(all_content.size() < body_size){
        if((recv_size = recv(sockfd, recv_str.data(), recv_str.size(), 0)) == -1){
            //TODO: exception handle
        }
        all_content += recv_str.substr(0, recv_size);
        recv_str = std::string(1024, '\0');
    }
    // close socket
    close(sockfd);
    return all_content;
}

std::pair<bool, std::vector<std::string>> HttpClient::move(int direction){

}
std::vector<std::string> HttpClient::restart(){

}
std::vector<std::string> HttpClient::start(std::string level){
    std::string body = send_request(std::string("/start?level=") + level);
    // Trim begin & end
    body = body.substr(1, body.size() - 2);
    // Split row
    std::vector<std::string> rows;
    
    return rows;
}
