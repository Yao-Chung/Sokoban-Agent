#include <iostream>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <utility>
#include <cstdio>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

#include <defines.hpp>

static void clean(int epollFd, std::unordered_map<int, std::pair<std::string, std::string> > &fifos){
    close(epollFd);
    for(auto [fd, pair]: fifos){
        close(fd);
        unlink(pair.first.c_str());
    }
}

static void stdin_handler(int epollFd, std::unordered_map<int, std::pair<std::string, std::string> > &fifos){
    std::string userInput;
    std::cin >> userInput;
    if(userInput == "quit"){
        // Quit
        clean(epollFd, fifos);
        exit(0);
    }else if(userInput == "help"){
        // Help
        std::cout << 
            "help: show help\n"
            "quit: quit"
        << std::endl;
    }else{
        std::cout << "Unknown command! Type \"help\" for help." << std::endl;
    }
}

static bool fifo_handler(int fifoFd, std::string &buffer){
    // Read data & put into buffer
    char buf[256];
    ssize_t size = 0;
    while ((size = read(fifoFd, buf, 256)) > 0){
        buffer += std::string(buf, size);
    }
    // Handle error
    if(size < 0){
        perror("read");
        return false;
    }
    // Extract data from buffer
    std::vector< std::pair<Map, std::vector<MoveDirection>> > solutions;
    while(buffer.size() > 8){
        int32_t *mapSize = (int32_t*)buffer.data();
        size_t mapBytes = mapSize[0] * mapSize[1];
        if(buffer.size() > (mapBytes + 8)){
            int32_t policySize = *(int32_t*)(buffer.data() + (mapBytes + 8));
            size_t solutionSize = mapBytes + policySize * sizeof(MoveDirection) + 12;
            if(buffer.size() >= solutionSize){
                std::stringstream stream(buffer);
                // Read map
                Map map = readMap(stream);
                // Read policy
                stream.read((char*)&policySize, sizeof(int32_t));
                std::vector<MoveDirection> policy(policySize);
                stream.read((char*)policy.data(), sizeof(MoveDirection) * policySize);
                // Push solutions
                solutions.emplace_back(map, policy);
                buffer = buffer.substr(solutionSize);
            }
        }
    }
    return true;
}

int main(int argc, char *argv[]){
    // Print usage
    if(argc < 4){
        std::cerr << "Usage: " << argv[0] << " <solutions_dir> [input_files]+" << std::endl;
        return -1;
    }
    // Ensure folder exists
    std::filesystem::path directory(argv[1]);
    if(std::filesystem::exists(directory)){
        if(!std::filesystem::is_directory(directory)){
            std::cerr << "'" << directory << "' is not a directory" << std::endl;
            return -1;
        }
    }else{
        std::filesystem::create_directories(directory);
    }
    // Create epoll queue
    int epollFd = epoll_create1(EPOLL_CLOEXEC);
    if(epollFd < 0){
        perror("epoll_create");
        return -1;
    }
    std::vector<epoll_event> events;
    // Create FIFOs
    std::unordered_map<int, std::pair<std::string, std::string> > fifos;
    for(int i = 2; i < argc; ++i){
        // Make
        if(mkfifo(argv[i], 0666)){
            perror("mkfifo");
            clean(epollFd, fifos);
            return -1;
        }
        // Open
        int fifoFd = open(argv[i], O_RDONLY | O_NONBLOCK);
        if(fifoFd == -1){
            perror("open");
            clean(epollFd, fifos);
            return -1;
        }
        // Add to epoll
        events.emplace_back(epoll_event{.events = EPOLLIN | EPOLLET, .data = {.fd = fifoFd}});
        if(epoll_ctl(epollFd, EPOLL_CTL_ADD, fifoFd, events.data() + (events.size() - 1))){
            perror("epoll_ctl");
            clean(epollFd, fifos);
            return -1;
        }
        // Add to fifos
        fifos.emplace(fifoFd, std::pair<std::string, std::string>(argv[i], ""));
    }
    // Add stdin to epoll
    events.emplace_back(epoll_event{.events = EPOLLIN, .data = {.fd = STDIN_FILENO}});
    if(epoll_ctl(epollFd, EPOLL_CTL_ADD, STDIN_FILENO, events.data() + (events.size() - 1))){
        perror("epoll_ctl[stdin]");
        clean(epollFd, fifos);
        return -1;
    }
    std::cout << "Welcome! Type \"help\" for help\n" << "> " << std::flush;
    // Event loop
    for(int eventCount = 0; (eventCount = epoll_wait(epollFd, events.data(), events.size(), -1)) >= 0; eventCount = 0){
        for(int i = 0; i < eventCount; ++i){
            epoll_event &event = events[i];
            if(event.data.fd == STDIN_FILENO){
                // stdin
                stdin_handler(epollFd, fifos);
                std::cout << "> " << std::flush;
            }else{
                // fifo
                if(event.events & EPOLLIN){
                    if(!fifo_handler(event.data.fd, fifos[event.data.fd].second)){
                        clean(epollFd, fifos);
                        return -1;
                    }
                }
            }
        }
    }
    // Error occured while epoll
    clean(epollFd, fifos);
    return -1;
}