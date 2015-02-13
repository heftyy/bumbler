cmake_minimum_required(VERSION 2.8.4)
project(bumbler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(BUMBLER_SOURCE_FILES
    bumbler/launcher/frame_servers.cpp
    bumbler/launcher/frame_servers.h
    bumbler/launcher/my_app.cpp
    bumbler/launcher/my_app.h
    bumbler/launcher/server_connection_actor.h
    bumbler/launcher/server_info.cpp
    bumbler/launcher/server_info.h
    bumbler/launcher/server_list.h
    bumbler/bumbler.cpp
    bumbler/out_actor.h
)

set(SERVER_SOURCE_FILES
    server/server.cpp
)

include_directories(shared)
include_directories(../boost_1_55_0)
add_subdirectory(shared/atan)

set(Boost_USE_STATIC_LIBS ${USE_STATIC_BOOST})
set(Boost_USE_MULTITHREADED ON)
set(BOOST_ROOT ../boost_1_55_0)
find_package(Boost REQUIRED COMPONENTS system serialization)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include( "${wxWidgets_USE_FILE}" )

add_executable(bumbler ${BUMBLER_SOURCE_FILES})
add_executable(server ${SERVER_SOURCE_FILES})

target_link_libraries(bumbler ${wxWidgets_LIBRARIES})
target_link_libraries(bumbler ${Boost_LIBRARIES})
target_link_libraries(bumbler atan)

target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server atan)