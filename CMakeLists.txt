cmake_minimum_required(VERSION 3.1)
project(bumbler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

#set(CMAKE_BUILD_TYPE Debug)
set(Boost_DEBUG 1)

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost 1.60 COMPONENTS system chrono thread date_time regex log log_setup serialization unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if(${WIN32})
  #windows
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

  #add_definitions(-DBOOST_LOG_DYN_LINK)
  add_definitions(-D_WIN32_WINNT=0x0601)
  SET(Boost_DETAILED_FAILURE_MSG  ON)
  SET(Boost_USE_STATIC_LIBS       ON)
  SET(Boost_USE_MULTITHREADED     ON)
  SET(Boost_USE_STATIC_RUNTIME    OFF)

  find_library(ws2_32_LIBRARY_PATH ws2_32)
ELSE()
  #linux
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")

  add_definitions(-std=c++14)
  #add_definitions(-DBOOST_ALL_DYN_LINK)

  #SET(Boost_USE_STATIC_RUNTIME  OFF)
  set(Boost_USE_STATIC_LIBS    ON)
  #set(Boost_USE_MULTITHREADED   ON)
ENDIF()

add_subdirectory(src/bumbler)

enable_testing()

include_directories(src)
include_directories(example/shared)
add_subdirectory(example/shared)
add_subdirectory(example/app)
add_subdirectory(example/server)
add_subdirectory(test)
