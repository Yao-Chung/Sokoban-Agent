#include <iostream>
#include <vector>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

#include <defines.hpp>
#include <Trainer.hpp>

static void clean(int epollFd, int fifoFd, std::string fifo){
    close(epollFd);
    close(fifoFd);
    unlink(fifo.c_str());
}

static void stdin_handler(int epollFd, int fifoFd, std::string fifo, Trainer &trainer){
    std::string userInput;
    std::cin >> userInput;
    if(userInput == "quit"){
        // Quit
        clean(epollFd, fifoFd, fifo);
        trainer.save();
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

static bool fifo_handler(int epollFd, int fifoFd, std::string fifo, std::string &buffer, Trainer &trainer){
    // Read data & put into buffer
    char buf[256];
    ssize_t size = 0;
    do{
        while ((size = read(fifoFd, buf, 256)) > 0){
            buffer += std::string(buf, size);
        }
        // Handle error
        if(size < 0){
            if(errno == EAGAIN){
                continue;
            }
            perror("read");
            return false;
        }
    }while(size > 0);
    // Read solutions from fifo
    std::vector< std::pair<Map, std::vector<MoveDirection>> > solutions;
    while(buffer.size() > 8){
        int32_t *mapSize = (int32_t*)buffer.data();
        size_t mapBytes = mapSize[0] * mapSize[1];
        if(buffer.size() > (mapBytes + 8)){
            int32_t policySize = *(int32_t*)(buffer.data() + (mapBytes + 8));
            size_t solutionSize = mapBytes + policySize * sizeof(MoveDirection) + 12;
            if(buffer.size() >= solutionSize){
                std::stringstream stream(buffer);
                Map map = readMap(stream);
                stream.read((char*)&policySize, sizeof(int32_t));
                std::vector<MoveDirection> policy(policySize);
                stream.read((char*)policy.data(), sizeof(MoveDirection) * policySize);
                // Push solutions
                solutions.emplace_back(map, policy);
                buffer = buffer.substr(solutionSize);
            }
        }
    }
    // Train
    for(auto [map, policy]: solutions){
        trainer.train(map, policy);
    }
    return true;
}

int main(int argc, char const *argv[])
{
    // Parse command-line arguments
    if(argc < 3){
        std::cerr << "Usage: " << argv[0] << " <solution_file> <model_file> [--stream]" << std::endl;
        return -1;
    }
    std::string fifo(argv[1]);
    Trainer trainer(argv[2]);
    if((argc > 3) && (std::string(argv[3]) == "--stream")){
        // Stream mode
        // Create epoll queue
        int epollFd = epoll_create1(EPOLL_CLOEXEC);
        if(epollFd < 0){
            perror("epoll_create");
            return -1;
        }
        std::vector<epoll_event> events;
        // Create FIFOs
        std::unordered_map<int, std::pair<std::string, std::string> > fifos;
        // Make
        if(mkfifo(fifo.c_str(), 0666)){
            perror("mkfifo");
            close(epollFd);
            return -1;
        }
        // Open
        int fifoFd = open(fifo.c_str(), O_RDONLY | O_NONBLOCK);
        if(fifoFd == -1){
            perror("open");
            close(epollFd);
            unlink(fifo.c_str());
            return -1;
        }
        // Add fifo to epoll
        std::string fifoBuf;
        events.emplace_back(epoll_event{.events = EPOLLIN | EPOLLET, .data = {.fd = fifoFd}});
        if(epoll_ctl(epollFd, EPOLL_CTL_ADD, fifoFd, events.data() + (events.size() - 1))){
            perror("epoll_ctl");
            clean(epollFd, fifoFd, fifo);
            return -1;
        }
        // Add stdin to epoll
        events.emplace_back(epoll_event{.events = EPOLLIN, .data = {.fd = STDIN_FILENO}});
        if(epoll_ctl(epollFd, EPOLL_CTL_ADD, STDIN_FILENO, events.data() + (events.size() - 1))){
            perror("epoll_ctl[stdin]");
            clean(epollFd, fifoFd, fifo);
            return -1;
        }
        std::cout << "Welcome! Type \"help\" for help\n" << "> " << std::flush;
        // Event loop
        for(int eventCount = 0; (eventCount = epoll_wait(epollFd, events.data(), events.size(), -1)) >= 0; eventCount = 0){
            for(int i = 0; i < eventCount; ++i){
                epoll_event &event = events[i];
                if(event.data.fd == STDIN_FILENO){
                    // stdin
                    stdin_handler(epollFd, fifoFd, fifo, trainer);
                    std::cout << "> " << std::flush;
                }else if(event.events & EPOLLIN){
                    // fifo
                    std::cout << "Receive solutions" << std::endl;
                    std::cout << "> " << std::flush;
                    if(!fifo_handler(epollFd, fifoFd, fifo, fifoBuf, trainer)){
                        clean(epollFd, fifoFd, fifo);
                        trainer.save();
                        return -1;
                    }
                }
            }
        }
        // Error occured while epoll
        clean(epollFd, fifoFd, fifo);
        trainer.save();
        return -1;
    }else{
        // Single file mode
        for(auto [map, policy]: read_solutions(argv[1])){
            trainer.train(map, policy);
        }
    }
    trainer.save();
    return 0;
}
