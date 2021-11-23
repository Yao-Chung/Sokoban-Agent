#include <iostream>
#include <defines.hpp>
#include <vector>
#include <ctime>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

#define SOLVER_NUM 1

// Return {rows, cols} and set file pointer to ready position
std::string random_map(std::ifstream &fp, std::vector<int> &startPos){
    // Get the random index of map
    int total = startPos.size();
    int index = rand() % total;
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
std::vector<int> get_start_pos(std::ifstream &fp){
    std::vector<int> startPos;
    while(!fp.eof()){
        startPos.push_back((int)fp.tellg());
        int32_t rows, cols;
        fp.read((char*)&rows, sizeof(int32_t));
        fp.read((char*)&cols, sizeof(int32_t));
        fp.seekg(rows*cols, std::ios_base::cur);
        fp.peek();
    }
    return startPos;
}
void handle_child(int nsig){
    // Wait for child
    pid_t child_pid = waitpid(0, NULL, 0);
    std::cerr << "A child with pid: " << child_pid << " is dead." << std::endl;
    //TODO:

}
int run_solver(char *solver_path, char **argv, std::ifstream &fp, std::vector<int> &startPos){
    // Get the random map for solver
    std::string returnMap = random_map(fp, startPos);
    // Create pipe
    int fd[2];
    int fd2[2];
    if(pipe(fd) == -1){
        perror("pipe failed");
        return -1;
    }
    if(pipe(fd2) == -1){
        perror("pipe2 failed");
        return -1;
    }
    pid_t child_pid = fork();
    if(child_pid == -1){
        perror("fork");
        return -1;
    }
    else if(child_pid > 0){     // This is parent process
        // Write the map into pipe
        write(fd[1], returnMap.data(), sizeof(returnMap));
        close(fd[1]);
    }else{      // This is child process
        dup2(fd[0], 0);
        dup2(fd2[1], 1);
        execve(solver_path, argv, nullptr);
        exit(0);
    }
    return fd2[0];
}
int main(int argc, char *argv[]){
    if(argc < 3){
        std::cerr << "Usage: " << argv[0] << "<map_file> <solver_file> [solution_file] [net_file]" << std::endl;
        return -1;
    }
    srand (time(NULL));
    std::string mapPath(argv[1]);
    std::string solverPath(argv[2]);
    std::string solutionFile(argv[3]);
    std::string netFile("");
    if(argc > 4){
        netFile = argv[4];
    }
    std::ifstream fp(mapPath, std::ios::binary);
    std::vector<int> startPos = get_start_pos(fp);
    // Install signal to sigaction
    struct sigaction act, oldact;
    memset(&act, 0, sizeof(act));
    act.sa_handler = handle_child;
    sigaddset(&act.sa_mask, SIGQUIT);
    sigaddset(&act.sa_mask, SIGINT);
    act.sa_flags = SA_NODEFER;
    if(sigaction(SIGCHLD, &act, &oldact) != 0){
        perror("sigaction() failed installing SIGINT handler.");
        return -1;
    }
    // Run solver
    std::vector<char*> solver_args;
    solver_args.emplace_back(solverPath.data());
    solver_args.emplace_back(solutionFile.data());
    if(netFile != ""){
        solver_args.emplace_back(netFile.data());
    }
    solver_args.emplace_back(nullptr);
    int fd = run_solver(solverPath.data(), solver_args.data(), fp, startPos);
    while(fd != -1){
        char buf[1024];
        int num = read(fd, buf, 1024);
        if(num > 0){
            std::string msg(buf, num);
            std::cout << msg << std::flush;
        }
            
    }
    return 0;
}