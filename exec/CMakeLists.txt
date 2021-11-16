add_executable(agent1
    agent1.cpp
)
target_link_libraries(agent1
    sokoban-agent
)

add_executable(trainer1
    trainer1.cpp
)
target_link_libraries(trainer1
    sokoban-agent
)