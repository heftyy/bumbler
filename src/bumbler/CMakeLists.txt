cmake_minimum_required(VERSION 3.1)

set(bumbler_SOURCES
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
        actor/messaging/messaging_service.cpp
        actor/messaging/messaging_service.h
        actor/channels/abstract_channel.h
        actor/channels/actor_channel.cpp
        actor/channels/actor_channel.h
        actor/channels/invalid_channel.h
        actor/channels/local_actor_channel.cpp
        actor/channels/local_actor_channel.h
        actor/channels/remote_actor_channel.cpp
        actor/channels/remote_actor_channel.h
        actor/local_actor.h
        actor/local_actor.cpp
        actor/promise_actor.h
        actor/abstract_actor.h
        actor/abstract_actor.cpp
        actor_system/actor_system.h
        actor_system/actor_system.cpp
        actor_system/actor_system_errors.h
        actor_system/udp_server.h
        actor_system/actor_system_storage.cpp
        actor/routing/router.cpp
        actor/routing/router.h
        actor/mailbox/mailbox.h
        actor/mailbox/standard_mailbox.h
        actor/mailbox/fifo_mailbox.h
        actor/mailbox/lifo_mailbox.h
        actor/mailbox/priority_mailbox.h
        actor/untyped_actor.h
        actor/props/typed_props.h
        actor/routing/router_logic.h
        actor/routing/smallest_mailbox_pool.cpp
        actor/routing/smallest_mailbox_pool.h
        actor/routing/round_robin_pool.cpp
        actor/routing/round_robin_pool.h
        actor/routing/random_pool.cpp
        actor/routing/random_pool.h
        actor/routing/router_pool.h
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
        actor/props/props.h
        actor/routing/router_pool.cpp
        actor_system/typed_promise_actor.h
        actor/promise_actor.cpp
        logger/logger.h
        logger/logger.cpp
        utility.h
        utility.cpp)

if (${WIN32})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_definitions(-D_WIN32_WINNT=0x0501)
    add_library(bumbler STATIC ${bumbler_SOURCES})
    target_link_libraries(bumbler ${Boost_LIBRARIES})
ELSE ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    add_library(bumbler STATIC ${bumbler_SOURCES})
    target_link_libraries(bumbler ${Boost_LIBRARIES} -lpthread)
ENDIF ()
