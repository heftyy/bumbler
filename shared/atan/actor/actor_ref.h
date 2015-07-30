#pragma once

#include <memory>
#include <iostream>
#include <sstream>
#include <future>
#include <logger/logger.h>
#include "atan/messages/typed_message.h"
#include "atan/messages/commands/commands.h"

class actor_ref {
public:
    std::string actor_name;
    std::string system_name;
    std::string ip;
    int port;

    actor_ref() : port(0) { }

    actor_ref(const std::string& actor_name, const std::string& system_name)
            : actor_name(actor_name), system_name(system_name), port(0) { }

    actor_ref(const std::string& actor_name, const std::string& system_name, const std::string& ip, const int port)
            : actor_name(actor_name), system_name(system_name), ip(ip), port(port) { }

    actor_ref(const std::string actor_ref) /** [actor]$[system_name]@[ip]:[port] */ {
        unsigned long long int system_name_start = actor_ref.find('$');
        unsigned long long int ip_start = actor_ref.find('@');
        unsigned long long int port_start = actor_ref.find(':');
        actor_name = actor_ref.substr(0, system_name_start);
        system_name = actor_ref.substr(system_name_start + 1, ip_start - system_name_start - 1);
        ip = actor_ref.substr(ip_start + 1, port_start - ip_start - 1);
        port = atoi(actor_ref.substr(port_start + 1, actor_ref.length() - port_start - 1).c_str());
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << actor_name << "$" << system_name << "@" << ip << ":" << port;
        return ss.str();
    }

    static actor_ref none() {
        return actor_ref();
    }

    //if a string literal is passed to tell i change it to std::string
    void tell(const char* data, actor_ref sender = actor_ref::none()) {
        tell(std::string(data), sender);
    }

    template<typename T>
    void tell(T data, actor_ref sender = actor_ref::none()) const {
        std::unique_ptr<typed_message<T>> msg_ptr = std::unique_ptr<typed_message<T>>(new typed_message<T>(*this, sender, data));
        actor_tell(std::move(msg_ptr));
    }

    template<typename T>
    void tell(broadcast<T> msg, actor_ref sender = actor_ref::none()) const {
        std::unique_ptr<typed_message<T>> msg_ptr = std::unique_ptr<typed_message<T>>(new typed_message<T>(*this, sender, msg));
        actor_tell(std::move(msg_ptr));
    }

    template<typename T>
    void tell(stop_actor<T> msg, actor_ref sender = actor_ref::none()) const {
        std::unique_ptr<typed_message<T>> msg_ptr = std::unique_ptr<typed_message<T>>(new typed_message<T>(*this, sender, msg));
        actor_tell(std::move(msg_ptr));
    }

    template<typename T>
    void tell(kill_actor<T> msg, actor_ref sender = actor_ref::none()) const {
        std::unique_ptr<typed_message<T>> msg_ptr = std::unique_ptr<typed_message<T>>(new typed_message<T>(*this, sender, msg));
        actor_tell(std::move(msg_ptr));
    }

    template<typename T>
    void tell(typed_message<T> msg) const {
        std::unique_ptr<typed_message<T>> msg_ptr = std::unique_ptr<typed_message<T>>(new typed_message<T>(msg));
        actor_tell(std::move(msg_ptr));
    }

    template<typename F>
    std::future<F> future(const char* data) const {
        return future<F>(std::string(data));
    }

    template<typename F, typename T>
    std::future<F> future(T data) const {
        auto promise_ptr = std::make_shared<std::promise<F>>();
        std::future<F> f = promise_ptr->get_future();

        size_t future_type_hashcode = typeid(F).hash_code();

        auto fn = [=](std::shared_ptr<std::promise<F>>& promise, boost::any response) {
            if(response.type().hash_code() == future_type_hashcode) {
                F value = boost::any_cast<F>(response);
                promise->set_value(value);
            }
            else {
                BOOST_LOG_TRIVIAL(error) << "[FUTURE] expected type for the future didn't match the replied type";
                promise->set_value(F());
            }
        };

        std::function<void(boost::any)> bound_fn = std::bind(fn, promise_ptr, std::placeholders::_1);

        std::unique_ptr<typed_message<T>> msg_ptr = std::unique_ptr<typed_message<T>>(new typed_message<T>(*this, none(), data));

        actor_future_tell(std::move(msg_ptr), bound_fn);

        return f;
    }

    /*
     * Stops the actor after all the received message are read.
     */
    void stop();

    /*
     * Discards all waiting messages and stops the actor as soon as possible.
     */
    void kill();

    bool is_none() const {
        return actor_name.length() == 0 || system_name.length() == 0;
    }

    bool is_remote() const {
        return actor_name.length() > 0 && system_name.length() > 0 && ip.length() > 0 && port > 0;
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & actor_name;
        ar & system_name;
        ar & ip;
        ar & port;
    }

private:
    void reset() {
        this->actor_name = "";
        this->system_name = "";
        this->ip = "";
        this->port = 0;
    }

    void actor_future_tell(std::unique_ptr<message> msg, std::function<void(boost::any)>& response_fn) const;
    void actor_tell(std::unique_ptr<message> msg) const;

};