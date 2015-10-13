cmake_minimum_required(VERSION 3.1)
project(atan)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")

set(ATAN_SOURCES
    messages/message.h
    messages/typed_message.h
    messages/serialization_utility.h
    messages/commands/command.h
    messages/commands/broadcast.h
    messages/commands/stop_actor.h
    messages/commands/kill_actor.h
    messages/commands/priority_message.h
    messages/commands/commands.h
    actor/actor_ref/actor_ref.h
    actor/actor_ref/actor_ref.cpp
    actor/remote_actor.h
    actor/remote_actor.cpp
    actor/local_actor.h
    actor/local_actor.cpp
    actor/promise_actor.h
    actor/promise_actor.cpp
    actor/actor.cpp
    actor_system/actor_system.h
    actor_system/actor_system.cpp
    actor_system/actor_system_errors.h
    actor_system/udp_server.h
    actor_system/actor_system_storage.cpp
    actor/router/router.cpp
    actor/router/router.h
    actor/router/random_router.cpp
    actor/router/random_router.h
    actor/router/round_robin_router.cpp
    actor/router/round_robin_router.h
    actor/mailbox/mailbox.h
    actor/mailbox/standard_mailbox.h
    actor/mailbox/fifo_mailbox.h
    actor/mailbox/lifo_mailbox.h
    actor/mailbox/priority_mailbox.h
    actor/untyped_actor.h
    packet/packet.h
    packet/packet_exceptions.h
    packet/packet_structs.h
    packet/packet_structs.cpp
    scheduler/scheduler.h
    scheduler/cancellable.h
    dispatcher/dispatcher.cpp
    dispatcher/dispatcher.h
    thread_pool/thread_pool.cpp
    thread_pool/thread_pool.h
)

include_directories(../)

if(${WIN32})
  add_definitions(-D_WIN32_WINNT=0x0501)
ELSE()
ENDIF()

add_library(atan STATIC ${ATAN_SOURCES})
target_link_libraries(atan ${Boost_LIBRARIES})
