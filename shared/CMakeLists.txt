cmake_minimum_required(VERSION 3.1)
project(shared)

set(SHARED_SOURCES
  communication/message_settings.h
  communication/serializable_types.h
  communication/server_status.h
  logger/logger.h
  logger/logger.cpp
  utility.h
  utility.cpp)

add_subdirectory(atan)

if(${WIN32})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	add_definitions(-D_WIN32_WINNT=0x0501)
	add_library(shared STATIC ${SHARED_SOURCES})
	target_link_libraries(shared atan)
ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	add_library(shared STATIC ${SHARED_SOURCES})
	target_link_libraries(shared atan -lpthread)
ENDIF()
