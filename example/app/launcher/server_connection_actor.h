#pragma once

#include <iostream>
#include <atomic>
#include <bumbler/actor/remote_actor.h>
#include <bumbler/actor_system/actor_system.h>
#include <communication/message_settings.h>

class server_connection_actor : public remote_actor {
public:
    server_connection_actor(std::string name, std::string remote_actor_ref, std::shared_ptr<actor_system> actor_system,
                            std::function<void(std::string)> update_server_function)
            : remote_actor(name, actor_system, actor_ref(remote_actor_ref)),
              update_server_function_(update_server_function) {
        //actor_system_.add_actor(std::shared_ptr<test_actor>(this));
    }

    ~server_connection_actor() {
        BOOST_LOG_TRIVIAL(debug) << "[SERVER_CONNECTION_ACTOR] destructor";
    }

private:

    std::function<void(std::string)> update_server_function_;

    template<typename T>
    void on_receive(T data) {
        BOOST_LOG_TRIVIAL(debug) << "[SERVER_CONNECTION_ACTOR] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "[SERVER_CONNECTION_ACTOR] received message from " << get_sender().actor_name;
        BOOST_LOG_TRIVIAL(debug) << "message was " << data;

        if (data == SERVER_STATUS) {
            BOOST_LOG_TRIVIAL(debug) << "[SERVER_CONNECTION_ACTOR] SERVER STATUS RECEIVED";
            update_server_function_(get_self().ip);
        }
    }
};

