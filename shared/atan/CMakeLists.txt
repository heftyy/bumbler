cmake_minimum_required(VERSION 3.1)
project(atan)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(ATAN_SOURCES
  messages/message.h
  messages/typed_message.h
  messages/serialization_utility.h
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
  packet/packet_structs.cpp
  scheduler/scheduler.h
  interruptible_thread.h
  )

add_definitions(-DBOOST_LOG_DYN_LINK)
add_library(atan STATIC ${ATAN_SOURCES})