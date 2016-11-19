cmake_minimum_required(VERSION 3.1)

set(SERVER_SOURCE_FILES
    server.cpp
    server_actor.h
)

add_executable(server ${SERVER_SOURCE_FILES})

target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server shared bumbler)
