#include <PolicyOptimize.hpp>
#include <Solver.hpp>
#include <unordered_map>

std::vector<MoveDirection> PolicyOptimize::optimize(const Map map, const std::vector<MoveDirection> policy){
    std::unordered_map<std::string, int> getIndex;
    std::vector<std::pair<std::string, MoveDirection>> stateSequence;
    Map curMap = map;
    for(MoveDirection dir: policy){
        std::string key = getKey(curMap);
        // Key repeat
        if(getIndex.contains(key)){
            int index = getIndex[key];
            // Delete the cycle
            for(int i=index; i<stateSequence.size(); i++){
                getIndex.erase(stateSequence[i].first);
            }
            stateSequence.resize(index);
        }
        stateSequence.push_back({key, dir});
        getIndex.emplace(key, stateSequence.size()-1);
        curMap = move(curMap, dir, map);
    }
    std::vector<MoveDirection> betterPolicy;
    for(auto p: stateSequence){
        betterPolicy.emplace_back(p.second);
    }
    return betterPolicy;
}