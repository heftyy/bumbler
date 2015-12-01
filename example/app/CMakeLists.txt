cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")

set(BUMBLER_SOURCE_FILES
    bumbler.cpp
    out_router.h
    out_actor.h
    remote_server_actor.h
)

add_executable(app ${BUMBLER_SOURCE_FILES})

target_link_libraries(app ${Boost_LIBRARIES})
target_link_libraries(app shared bumbler)
