cmake_minimum_required(VERSION 3.1)
project(bumbler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(BUMBLER_SOURCE_FILES
#    bumbler/launcher/frame_servers.cpp
#    bumbler/launcher/frame_servers.h
#    bumbler/launcher/my_app.cpp
#    bumbler/launcher/my_app.h
#    bumbler/launcher/server_connection_actor.h
#    bumbler/launcher/server_info.cpp
#    bumbler/launcher/server_info.h
#    bumbler/launcher/server_list.h
    bumbler/out_router.h
    bumbler/bumbler.cpp
    bumbler/out_actor.h
    bumbler/remote_server_actor.h
    shared/logger/logger.cpp
)

set(SERVER_SOURCE_FILES
    server/server_actor.h
    server/server.cpp
)

set(CMAKE_BUILD_TYPE Debug)
SET(Boost_DEBUG ON)

if(${WIN32})
    #windows
    include_directories(F:/github/boost_1_58_0)

    #add_definitions(-DBOOST_LOG_DYN_LINK)
    add_definitions(-D_WIN32_WINNT=0x0501)
    set(BOOST_ROOT F:/github/boost_1_58_0)
    SET(Boost_DETAILED_FAILURE_MSG ON)
    SET(Boost_USE_STATIC_LIBS      ON)
    SET(Boost_USE_MULTITHREADED    ON)
    SET(Boost_USE_STATIC_RUNTIME   OFF)

    find_package(Boost COMPONENTS system thread chrono date_time regex filesystem log log_setup serialization unit_test_framework REQUIRED)
    find_library(ws2_32_LIBRARY_PATH ws2_32)

    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY F:/github/bumbler/bin)
ELSE()
    #linux
    include_directories(../boost_1_58_0)

    add_definitions(-DBOOST_LOG_DYN_LINK)
    add_definitions(-DBOOST_TEST_DYN_LINK)

    #set(Boost_USE_STATIC_LIBS   ON)
    set(Boost_USE_MULTITHREADED ON)
    set(BOOST_ROOT ../boost_1_58_0)
    find_package(Boost COMPONENTS thread system chrono date_time regex filesystem log log_setup serialization unit_test_framework REQUIRED)
ENDIF()

include_directories(shared)
add_subdirectory(shared)

add_executable(bumbler ${BUMBLER_SOURCE_FILES})
add_executable(server ${SERVER_SOURCE_FILES})

#target_link_libraries(bumbler ${wxWidgets_LIBRARIES})
target_link_libraries(bumbler ${Boost_LIBRARIES})
target_link_libraries(bumbler shared)
#target_link_libraries(bumbler ${ws2_32_LIBRARY_PATH})

target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server shared)
#target_link_libraries(server ${ws2_32_LIBRARY_PATH})


enable_testing()

#I like to keep test files in a separate source directory called test
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/*.cpp)

#Run through each source
foreach(testSrc ${TEST_SRCS})
    #Extract the filename without an extension (NAME_WE)
    get_filename_component(testName ${testSrc} NAME_WE)

    #Add compile target
    add_executable(${testName} ${testSrc})

    #link to Boost libraries AND your targets and dependencies
    target_link_libraries(${testName} ${Boost_LIBRARIES} shared)

    #I like to move testing binaries into a testBin directory
    set_target_properties(${testName} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/testBin)

    #Finally add it to test execution -
    #Notice the WORKING_DIRECTORY and COMMAND
    add_test(NAME ${testName}
             WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin
             COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/${testName} )
endforeach(testSrc)