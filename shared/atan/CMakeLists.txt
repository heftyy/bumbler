cmake_minimum_required(VERSION 2.8.4)
project(atan)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(../)

set(ATAN_SOURCES
  actor/actor.cpp
  actor/remote_actor.cpp
  actor/local_actor.cpp
  actor/actor_ref.cpp
  actor_system/actor_system_storage.cpp
  actor/actor_ref.h
  actor/local_actor.h
  actor_system/actor_system.h
  actor_system/actor_system_errors.h
  actor_system/udp_server.h
  packet/packet.h
  packet/packet_exceptions.h
  packet/packet_structs.h
  scheduler/scheduler.h
  interruptible_thread.h
  message.h
  )

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
add_library(atan SHARED ${ATAN_SOURCES})