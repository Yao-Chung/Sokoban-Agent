#include <MarklovSolver/PolicyVisualizer.hpp>

PolicyVisualizer::PolicyVisualizer(std::string prefix, std::string extension): Visualizer(prefix, extension)
{}

void PolicyVisualizer::visualize(
    const std::vector<MoveDirection> &policy,
    const State* root
){
    // Print
    out << "digraph{" << std::endl;
    const State* state = root;
    for(MoveDirection dir: policy){
        // Print state
        out << "\tm" << state << "[label=\""
            << "Dis=" << state->distance << "\\n"
            << "Tar=" << state->finishTargets << "\\n"
            << "Man=(" << state->manPosition.first << ", " << state->manPosition.second << ")\\n"
        << "\"";
        out << "]" << std::endl;
        // Print actions
        for(Action* action: state->actions){
            if(action->direction == dir){
                out << "\tm" << action->parent << " -> m" << action->next << "[label=\""
                    << "P=" << action->pathCost << "\\n"
                    << "R=" << action->restartCost << "\\n"
                    << "Con=" << action->confidence << "\\n";
                switch (action->direction){
                    case MoveDirection::Up:
                        out << "Dir=Up\\n";
                        break;
                    case MoveDirection::Down:
                        out << "Dir=Down\\n";
                        break;
                    case MoveDirection::Left:
                        out << "Dir=Left\\n";
                        break;
                    case MoveDirection::Right:
                        out << "Dir=Right\\n";
                        break;
                }
                out << "\"";
                out << "]" << std::endl;
                state = action->next;
                break;
            }
        }
    }
    // Epilogue
    out << "}" << std::endl;
}