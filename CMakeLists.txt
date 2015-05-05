cmake_minimum_required(VERSION 3.2)
project(bumbler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(BUMBLER_SOURCE_FILES
#    bumbler/launcher/frame_servers.cpp
#    bumbler/launcher/frame_servers.h
#    bumbler/launcher/my_app.cpp
#    bumbler/launcher/my_app.h
#    bumbler/launcher/server_connection_actor.h
#    bumbler/launcher/server_info.cpp
#    bumbler/launcher/server_info.h
#    bumbler/launcher/server_list.h
    bumbler/bumbler.cpp
    bumbler/out_actor.h
    shared/utility.cpp
)

set(SERVER_SOURCE_FILES
    server/server.cpp
)

set(CMAKE_BUILD_TYPE Debug)

if(${WIN32})
    #windows
    include_directories(F:/github/boost_1_58_0)
    include_directories(shared)
    add_subdirectory(shared/atan)

    add_definitions()
    add_definitions(-DBOOST_LOG_DYN_LINK)
    add_definitions(-D_WIN32_WINNT=0x0501)
    set(BOOST_ROOT F:/github/boost_1_58_0)
    set(Boost_USE_STATIC_LIBS   ON)
    set(Boost_USE_MULTITHREADED ON)

    find_package(Boost 1.58 COMPONENTS system date_time regex filesystem log log_setup serialization REQUIRED)
    find_library(ws2_32_LIBRARY_PATH ws2_32)

#    set(wxWidgets_ROOT_DIR "F:/github/wxWidgets")
#    set(wxWidgets_LIB_DIR "F:/github/wxWidgets/lib/gcc_dll")
#    find_package(wxWidgets COMPONENTS REQUIRED core base)
#    include( "${wxWidgets_USE_FILE}" )
ELSE()
    #linux
    include_directories(../boost_1_55_0)
    include_directories(shared)
    add_subdirectory(shared/atan)

    add_definitions(-DBOOST_LOG_DYN_LINK)
    set(Boost_USE_STATIC_LIBS   ON)
    set(Boost_USE_MULTITHREADED ON)
    set(BOOST_ROOT ../boost_1_55_0)
    find_package(Boost COMPONENTS system date_time regex filesystem log log_setup serialization REQUIRED)

    find_package(wxWidgets COMPONENTS core base REQUIRED)
    include( "${wxWidgets_USE_FILE}" )
ENDIF()

add_executable(bumbler ${BUMBLER_SOURCE_FILES})
add_executable(server ${SERVER_SOURCE_FILES})

#target_link_libraries(bumbler ${wxWidgets_LIBRARIES})
target_link_libraries(bumbler ${Boost_LIBRARIES})
target_link_libraries(bumbler atan)
target_link_libraries(bumbler ${ws2_32_LIBRARY_PATH})

target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server atan)
target_link_libraries(server ${ws2_32_LIBRARY_PATH})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY F:/github/bumbler/bin)