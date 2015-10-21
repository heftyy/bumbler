cmake_minimum_required(VERSION 3.1)
project(bumbler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")

set(BUMBLER_SOURCE_FILES
    bumbler.cpp
    out_router.h
    out_actor.h
    remote_server_actor.h
)
add_executable(bumbler ${BUMBLER_SOURCE_FILES})

target_link_libraries(bumbler ${Boost_LIBRARIES})
target_link_libraries(bumbler shared)