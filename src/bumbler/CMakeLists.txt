cmake_minimum_required(VERSION 3.1)

set(bumbler_SOURCES
        messages/variant.h
        messages/typed_variant.h
        messages/message.h
        messages/typed_message.cpp
        messages/typed_message.h        
        messages/serialization_utility.h
        messages/commands/command.h
        messages/commands/broadcast.h
        messages/commands/stop_actor.h
        messages/commands/kill_actor.h
        messages/commands/priority_message.h
        messages/commands/commands.h
        actor/actor_ref/actor_ref.cpp
        actor/actor_ref/actor_ref.h        
        actor/channels/abstract_channel.h
        actor/channels/actor_channel.h
        actor/channels/invalid_channel.h
        actor/channels/local_actor_channel.cpp
        actor/channels/local_actor_channel.h
        actor/channels/remote_actor_channel.cpp
        actor/channels/remote_actor_channel.h
        actor/local_actor.cpp
        actor/local_actor.h
        actor/promise_actor.cpp
        actor/promise_actor.h
        actor/abstract_actor.cpp
        actor/abstract_actor.h
        actor/untyped_actor.h
        actor/typed_promise_actor.h
        actor_system/actor_system.cpp
        actor_system/actor_system.h
        actor_system/actor_system_errors.h
        actor_system/udp_server.h
        actor_system/actor_system_storage.cpp
        actor_system/actor_system_storage.h
        actor/routing/router.cpp
        actor/routing/router.h
        actor/mailbox/mailbox.h
        actor/mailbox/standard_mailbox.h
        actor/mailbox/fifo_mailbox.cpp
        actor/mailbox/fifo_mailbox.h
        actor/mailbox/lifo_mailbox.cpp
        actor/mailbox/lifo_mailbox.h
        actor/mailbox/priority_mailbox.cpp
        actor/mailbox/priority_mailbox.h
        actor/props/typed_props.h
        actor/props/props.h
        actor/routing/router_logic.h
        actor/routing/smallest_mailbox_pool.cpp
        actor/routing/smallest_mailbox_pool.h
        actor/routing/round_robin_pool.cpp
        actor/routing/round_robin_pool.h
        actor/routing/random_pool.cpp
        actor/routing/random_pool.h
        actor/routing/router_pool.cpp
        actor/routing/router_pool.h
        packet/packet.h
        packet/packet_exceptions.h
        packet/packet_structs.cpp
        packet/packet_structs.h
        scheduler/scheduler.cpp
        scheduler/scheduler.h
        scheduler/cancellable.h
        dispatcher/dispatcher.h
        thread_pool/thread_pool.cpp
        thread_pool/thread_pool.h
        logger/logger.h
        logger/logger.cpp
        utility.h
        utility.cpp
        internal/identifier.h
        internal/bumbler.h
        internal/messaging_service.cpp
        internal/messaging_service.h
        )

if (${WIN32})
    add_library(bumbler STATIC ${bumbler_SOURCES})
    target_link_libraries(bumbler ${Boost_LIBRARIES})
ELSE ()
    add_definitions(-std=c++14)
    add_library(bumbler STATIC ${bumbler_SOURCES})
    target_link_libraries(bumbler ${Boost_LIBRARIES} -lpthread)
ENDIF ()
