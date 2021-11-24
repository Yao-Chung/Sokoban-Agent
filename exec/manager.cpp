#include <iostream>
#include <defines.hpp>
#include <vector>
#include <ctime>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <cstring>
#include <unordered_map>

std::unordered_map<pid_t, char**> solver_parameter;
std::ifstream fp;
std::vector<int> startPos;

// Return {rows, cols} and set file pointer to ready position
std::string random_map(){
    // Get the random index of map
    int total = startPos.size();
    int index = rand() % total;
    std::cout << "Map index: " << index << std::endl;
    // Point the file pointer to target map
    fp.seekg(startPos[index], std::ios_base::beg);
    int32_t rows, cols;
    fp.read((char*)&rows, sizeof(int32_t));
    fp.read((char*)&cols, sizeof(int32_t));
    fp.seekg(startPos[index], std::ios_base::beg);
    std::string returnMap(rows*cols+8, ' ');
    fp.read((char*)returnMap.data(), rows*cols+8);
    return returnMap;
}
void get_start_pos(){
    while(!fp.eof()){
        startPos.push_back((int)fp.tellg());
        int32_t rows, cols;
        fp.read((char*)&rows, sizeof(int32_t));
        fp.read((char*)&cols, sizeof(int32_t));
        fp.seekg(rows*cols, std::ios_base::cur);
        fp.peek();
    }
}
void run_solver(char **argv){
    // Get the random map for solver
    std::string returnMap = random_map();
    // Create pipe
    int fd[2];
    if(pipe(fd) == -1){
        perror("pipe failed");
        return;
    }
    pid_t child_pid = fork();
    if(child_pid == -1){
        perror("fork");
        return;
    }
    else if(child_pid > 0){     // This is parent process
        // Write the map into pipe
        solver_parameter[child_pid] = argv;
        close(fd[0]);
        write(fd[1], returnMap.data(), returnMap.size());
        close(fd[1]);
    }else{      // This is child process
        close(fd[1]);
        dup2(fd[0], 0);
        execve(argv[0], argv, nullptr);
        exit(0);
    }
}
void handle_child(int nsig){
    // Wait for child
    int status = 0;
    pid_t child_pid = waitpid(0, &status, 0);
    if(child_pid == -1){
        perror("waitpid error");
        return;
    }
    std::cerr << "A child with pid: " << child_pid << " is dead." << std::endl;
    if(WIFSIGNALED(status)){
        std::cout << "child was killed by a signal." << std::endl;
        return;
    }
    char **argv = solver_parameter[child_pid];
    run_solver(argv);
}

static void clean(){
    // Clean child
    while(solver_parameter.size() > 0){
        auto p = *solver_parameter.begin();
        kill(p.first, SIGKILL);
        solver_parameter.erase(p.first);
    }
}

int main(int argc, char *argv[]){
    if(argc < 5){
        std::cerr << "Usage: " << argv[0] << "<map_file> <solver_file> <child_num> [solution_file]+ [net_file]" << std::endl;
        return -1;
    }
    srand (time(NULL));
    std::string mapPath(argv[1]);
    std::string solverPath(argv[2]);
    std::string child_num(argv[3]);
    int child_number = std::stoi(child_num);
    std::vector<std::string> solutionFiles;
    // Get all solution files from argv
    for(int i=0; i<child_number; ++i){
        solutionFiles.push_back(argv[4+i]);
    }
    std::string netFile("");
    if(argc > (4 + child_number)){
        netFile = argv[4 + child_number];
    }
    // Initialize fp and startPos
    fp = std::ifstream(mapPath, std::ios::binary);
    startPos.resize(0);
    get_start_pos();
    // Install signal to sigaction
    struct sigaction act, oldact;
    memset(&act, 0, sizeof(act));
    act.sa_handler = handle_child;
    sigaddset(&act.sa_mask, SIGQUIT);
    sigaddset(&act.sa_mask, SIGINT);
    act.sa_flags = SA_NODEFER;
    // Error check
    if(sigaction(SIGCHLD, &act, &oldact) != 0){
        perror("sigaction() failed installing SIGINT handler.");
        return -1;
    }
    // Create epoll queue
    int epollFd = epoll_create1(EPOLL_CLOEXEC);
    if(epollFd < 0){
        perror("epoll_create");
        return -1;
    }
    epoll_event event {.events=EPOLLIN, .data = {.fd = STDIN_FILENO}};
    // Add stdin to epoll
    if(epoll_ctl(epollFd, EPOLL_CTL_ADD, STDIN_FILENO, &event)){
        perror("epoll_ctl[stdin]");
        return -1;
    }
    // Run solver
    std::vector<char*> solver_args;
    solver_args.emplace_back(solverPath.data());
    solver_args.emplace_back(std::string("").data());
    if(netFile != ""){
        solver_args.emplace_back(netFile.data());
    }
    solver_args.emplace_back(nullptr);
    
    for(int i=0; i<child_number; ++i){
        solver_args[1] = solutionFiles[i].data();
        run_solver(solver_args.data());
    }
    std::cout << "Type quit to exit the parent process" << std::endl;
    std::cout << "> " << std::flush;
    for(int eventCount = 0; ((eventCount = epoll_wait(epollFd, &event, 1, -1)) >= 0) || (errno == EINTR); eventCount = 0){
        for(int i = 0; i < eventCount; ++i){
            if(event.events & EPOLLIN){
                std::string command;
                std::cin >> command;
                if(command == "quit"){
                    clean();
                    std::cout << "Bye!" << std::endl;
                    return 0;
                }
                std::cout << "> " << std::flush;
            }
        }
    }
    perror("epoll_wait");
    clean();
    return 0;
}