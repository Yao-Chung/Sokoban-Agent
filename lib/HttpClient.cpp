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

static std::vector<std::string> parseMap(std::string body){
    std::vector<std::string> rows;
    std::string row;
    for(size_t pos = body.find("],["); pos != std::string::npos; pos = body.find("],[")){
        std::string trimmed = body.substr(0, pos);
        for(size_t comma = trimmed.find(","); comma != std::string::npos; comma = trimmed.find(",")){
            row += trimmed.substr(0, comma).substr(1, 1);
            trimmed = trimmed.substr(comma + 1);
        }
        row += trimmed.substr(1, 1);
        rows.emplace_back(row);
        row = std::string();
        body = body.substr(pos + 3);
    }
    // Remaining
    for(size_t comma = body.find(","); comma != std::string::npos; comma = body.find(",")){
        row += body.substr(0, comma).substr(1, 1);
        body = body.substr(comma + 1);
    }
    row += body.substr(1, 1);
    rows.emplace_back(row);
    return rows;
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
    msg += (url + " HTTP/1.1\r\n");
    if(cookie == ""){
        msg += "\r\n";
    }
    else{
        msg = msg + "Cookie: " + cookie + "\r\n\r\n";
    }
    if (send(sockfd, msg.c_str(), msg.size(), 0) == -1) 
    {
        //TODO: exception handle
    }
    // receive message
    std::string recv_str(MAXDATASIZE, '\0');
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
            // Get cookie
            remain = all_content.substr(all_content.find("Set-cookie: ")+12);
            cookie = remain.substr(0, remain.find("\r"));
            //Get body
            all_content = all_content.substr(index+4);
            recv_str = std::string(MAXDATASIZE, '\0');
            break;
        }
        recv_str = std::string(MAXDATASIZE, '\0');
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
        recv_str = std::string(MAXDATASIZE, '\0');
    }
    // close socket
    close(sockfd);
    return all_content;
}
std::pair<bool, std::vector<std::string>> HttpClient::move(int direction){

}
std::vector<std::string> HttpClient::restart(){
    std::string body = send_request("/restart");
    // Trim begin & end
    body = body.substr(2, body.size() - 4);
    return parseMap(body);
}
std::vector<std::string> HttpClient::start(std::string level){
    std::string body = send_request(std::string("/start?level=") + level);
    // Trim begin & end
    body = body.substr(2, body.size() - 4);
    return parseMap(body);
}
