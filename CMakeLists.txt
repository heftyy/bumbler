cmake_minimum_required(VERSION 3.1)
project(bumbler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

set(CMAKE_BUILD_TYPE Debug)
set(Boost_DEBUG ON)

if(${WIN32})
  #windows
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  include_directories(E:/Workspace/boost_1_59_0)

  #add_definitions(-DBOOST_LOG_DYN_LINK)
  add_definitions(-D_WIN32_WINNT=0x0501)
  set(BOOST_ROOT E:/Workspace/boost_1_59_0)
  SET(Boost_DETAILED_FAILURE_MSG ON)
  SET(Boost_USE_STATIC_LIBS      ON)
  SET(Boost_USE_MULTITHREADED    ON)
  SET(Boost_USE_STATIC_RUNTIME   OFF)

  find_package(Boost 1.59 COMPONENTS system thread chrono date_time regex filesystem log log_setup serialization unit_test_framework REQUIRED)
  find_library(ws2_32_LIBRARY_PATH ws2_32)

  #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY E:/Workspace/github/bumbler/bin)
ELSE()
  #linux
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")
  include_directories(../boost_1_59_0)

  #add_definitions(-DBOOST_TEST_DYN_LINK)

  add_definitions(-DBOOST_ALL_DYN_LINK)
  SET(Boost_USE_STATIC_RUNTIME   OFF)
  #set(Boost_USE_STATIC_LIBS   ON)
  set(Boost_USE_MULTITHREADED ON)
  set(BOOST_ROOT ../boost_1_59_0)
  find_package(Boost 1.59 COMPONENTS system thread chrono date_time regex filesystem log log_setup serialization unit_test_framework REQUIRED)
ENDIF()

include_directories(shared)
add_subdirectory(shared)

enable_testing()

add_subdirectory(bumbler)
add_subdirectory(server)
add_subdirectory(test)
