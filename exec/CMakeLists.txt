add_executable(solver
    solver.cpp
)
target_link_libraries(solver
    sokoban-agent
    ${TORCH_LIBRARIES}
)

add_executable(agent
    agent.cpp
)
target_link_libraries(agent
    sokoban-agent
    ${TORCH_LIBRARIES}
)

add_executable(trainer
    trainer.cpp
)
target_link_libraries(trainer
    sokoban-agent
    ${TORCH_LIBRARIES}
)

add_executable(collector
    collector.cpp
)
target_link_libraries(collector
    sokoban-agent
)

add_executable(extract
    extract.cpp
)
target_link_libraries(extract
    sokoban-agent
)

add_executable(manager
    manager.cpp
)
target_link_libraries(manager
    sokoban-agent
    ${TORCH_LIBRARIES}
)

add_executable(organizer
    organizer.cpp
)
target_link_libraries(organizer
    sokoban-agent
)
