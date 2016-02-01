#pragma once

#include <memory>
#include <iostream>
#include <sstream>
#include <future>
#include "../channels/abstract_channel.h"
#include "../../logger/logger.h"
#include "../../messages/typed_message.h"
#include "../../messages/commands/commands.h"
#include "../../utility.h"

namespace bumbler {

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

    actor_ref(const std::string& actor_ref) /** [actor]$[system_name]@[ip]:[port] */ {
	    auto system_name_start = actor_ref.find('$');
	    auto ip_start = actor_ref.find('@');
        unsigned long long int port_start = actor_ref.find(':');
        actor_name = actor_ref.substr(0, system_name_start);
        system_name = actor_ref.substr(system_name_start + 1, ip_start - system_name_start - 1);
        ip = actor_ref.substr(ip_start + 1, port_start - ip_start - 1);
        //boost asio doesn't like localhost
        if(ip.compare("localhost") == 0) ip = "127.0.0.1";
        port = atoi(actor_ref.substr(port_start + 1, actor_ref.length() - port_start - 1).c_str());
    }

    virtual ~actor_ref() = default; // make dtor virtual
    actor_ref(actor_ref&&) = default; // support moving
    actor_ref& operator=(actor_ref&&) = default;
    actor_ref(const actor_ref& rhs) {
        this->actor_name = rhs.actor_name;
        this->system_name = rhs.system_name;
        this->ip = rhs.ip;
        this->port = rhs.port;
    }
    actor_ref& operator=(const actor_ref& rhs) {
        this->actor_name = rhs.actor_name;
        this->system_name = rhs.system_name;
        this->ip = rhs.ip;
        this->port = rhs.port;
        return *this;
    }

    std::string to_string() const;

    static actor_ref none() {
        return actor_ref();
    }

    //if a string literal is passed to tell change it to std::string
    void tell(const char* data, actor_ref sender = actor_ref::none()) {
        this->tell(std::string(data), sender);
    }

    template<typename T>
    void tell(T&& data, const actor_ref& sender = actor_ref::none()) {
        auto tm = typed_message_factory::create(*this, sender, std::forward<T>(data));
        if(!channel_) resolve();

        channel_->tell(std::move(tm));
    }

    template<typename T>
    void tell(const typed_message<T>& msg) {
        if(!channel_) resolve();
        channel_->tell(std::move(utility::make_unique<typed_message<T>>(std::move(msg))));
    }

    template<typename Result>
    std::future<Result> ask(const char* data) {
        return ask<Result>(std::string(data));
    }

    template<typename Result, typename T>
    std::future<Result> ask(T && data) {
        auto tm = typed_message_factory::create(*this, none(), std::forward<T>(data));
        if(!channel_) resolve();

        return channel_->ask<Result>(std::move(tm));
    }

    /*
     * Creates the underlying abstract_channel
     */
    void resolve();

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

    bool is_resolved() const {
        return channel_ != nullptr;
    }

    bool operator==(const actor_ref& ref) const {
        return this->to_string().compare(ref.to_string()) == 0;
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & actor_name;
        ar & system_name;
        ar & ip;
        ar & port;
    }

private:
    std::unique_ptr<abstract_channel> channel_;

    void reset() {
        this->actor_name = "";
        this->system_name = "";
        this->ip = "";
        this->port = 0;
    }

};

}