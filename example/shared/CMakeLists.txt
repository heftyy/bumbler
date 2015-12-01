cmake_minimum_required(VERSION 3.1)

set(SHARED_SOURCES
  communication/message_settings.h
  communication/serializable_types.h
  communication/server_status.h
  communication/server_status.cpp
  )

if(${WIN32})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	add_definitions(-D_WIN32_WINNT=0x0501)
	add_library(shared STATIC ${SHARED_SOURCES})
ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	add_library(shared STATIC ${SHARED_SOURCES})
ENDIF()
