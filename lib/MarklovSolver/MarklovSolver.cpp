#include <MarklovSolver/MarklovSolver.hpp>
#include <queue>

MarklovSolver::MarklovSolver(
    float alpha,
    float beta,
    float gamma,
    unsigned int iter,
    Map map
): Solver(map), alpha(alpha), beta(beta), gamma(gamma), iter(iter), map(map){

}

std::vector<MoveDirection> MarklovSolver::solve(){
    // initialize the root state and put into hash map
    State* curState = new State(0, getMap());
    allStates[curState->key] = curState;
    // walk from root state
    MoveDirection dirs[] = {MoveDirection::Up, MoveDirection::Down, MoveDirection::Left, MoveDirection::Right};
    bool finished = false;
    while(!finished){
        // create states and actions
        for(MoveDirection dir: dirs){
            Map newMap = move(map, dir);
            // check if is win or not
            if(isWin(newMap)){
                finished = true;
                break;
            }
            // check if newState equals to curState
            if(curState->key == State::getKey(newMap)){
                continue;
            }
            // create new state
            State* newState = new State(curState->distance + 1, newMap);
            allStates[newState->key] = newState;
            // Create curState's action
            Action& action = curState->actions.emplace_back();
            action.parent = curState;
            action.next = newState;
            action.direction = dir;
            // curState.actions.push_back();
        }


    }
    // std::queue<State> q;
    // q.push(rootState);
    // int distance = 1;
    // while(!q.empty()){
    //     int num = q.size();
    //     for(int i=0; i<num; i++){
    //         State curState = q.front();
    //         q.pop();
    //         for(MoveDirection dir: dirs){
    //             Map newMap = move(map, dir);
    //             State newState = State();
    //         }
    //     }
    // }


}

void MarklovSolver::restart(){

}

void MarklovSolver::update(){

}