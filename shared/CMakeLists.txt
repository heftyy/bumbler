cmake_minimum_required(VERSION 3.1)
project(shared)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SHARED_SOURCES
  communication/message_settings.h
  communication/serializable_types.h
  communication/server_status.h
  logger/logger.h
  logger/logger.cpp
  utility.h
  utility.cpp
  )

add_subdirectory(atan)

add_library(shared STATIC ${SHARED_SOURCES})
target_link_libraries(shared atan)
