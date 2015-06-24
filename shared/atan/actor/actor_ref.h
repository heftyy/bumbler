#pragma once

#include <memory>
#include <iostream>
#include <sstream>
#include "atan/messages/typed_message.h"

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
        std::unique_ptr<typed_message<std::string>> msg = std::unique_ptr<typed_message<std::string>>(new typed_message<std::string>(*this, sender, std::string(data)));
        tell_(std::move(msg));
    }

    template<typename T>
    void tell(T data, actor_ref sender = actor_ref::none()) {
        std::unique_ptr<typed_message<T>> msg = std::unique_ptr<typed_message<T>>(new typed_message<T>(*this, sender, data));
        tell_(std::move(msg));
    }

    template<typename T>
    void tell(typed_message<T>& message) {
        std::unique_ptr<typed_message<T>> msg = std::unique_ptr<typed_message<T>>(new typed_message<T>(message));
        tell_(std::move(msg));
    }

    bool exists() const {
        return actor_name.length() > 0 && system_name.length() > 0;
    }

    bool valid_address() const {
        return actor_name.length() > 0 && system_name.length() > 0 && ip.length() > 0 && port > 0;
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& actor_name;
        ar& system_name;
        ar& ip;
        ar& port;
    }

private:
    void tell_(std::unique_ptr<message> msg);

};