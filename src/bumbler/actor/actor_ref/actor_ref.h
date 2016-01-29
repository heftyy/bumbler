#pragma once

#include <memory>
#include <iostream>
#include <sstream>
#include <future>
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
    actor_ref(const actor_ref&) = default; // support copying
    actor_ref& operator=(const actor_ref&) = default;

    std::string to_string() const {
        std::stringstream ss;
        ss << actor_name << "$" << system_name << "@" << ip << ":" << port;
        return ss.str();
    }

    static actor_ref none() {
        return actor_ref();
    }

    //if a string literal is passed to tell change it to std::string
    void tell(char* data, actor_ref sender = actor_ref::none()) {
        this->tell(std::string(data), sender);
    }

    template<typename T>
    void tell(T&& data, actor_ref sender = actor_ref::none()) const {
        auto tm = typed_message_factory::create(*this, sender, std::forward<T>(data));
		auto tm_ptr = utility::make_unique<decltype(tm)>(std::move(tm));
        tell_impl(std::move(tm_ptr));
    }

    template<typename T>
    void tell(const typed_message<T>& msg) const {
        tell_impl(std::move(utility::make_unique<typed_message<T>>(std::move(msg))));
    }

    template<typename Result>
    std::future<Result> ask(const char* data) const {
        return ask<Result>(std::string(data));
    }

    template<typename Result, typename T>
    std::future<Result> ask(T && data) const {
        auto promise_ptr = std::make_shared<std::promise<Result>>();
        auto f = promise_ptr->get_future();

	    auto future_type_hashcode = typeid(Result).hash_code();

        std::function<void(boost::any)> future_func = [future_type_hashcode, promise_ptr](boost::any response) {
            if(response.type().hash_code() == future_type_hashcode) {
                Result value = boost::any_cast<Result>(response);
                promise_ptr->set_value(value);
            }
            else {
                BOOST_LOG_TRIVIAL(error) << "[FUTURE] expected type for the future didn't match the replied type";
                promise_ptr->set_value(Result());
            }
        };

        auto tm = typed_message_factory::create(*this, none(), std::forward<T>(data));
        auto tm_ptr = utility::make_unique<decltype(tm)>(std::move(tm));

        ask_impl(std::move(tm_ptr), future_func);

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
    void reset() {
        this->actor_name = "";
        this->system_name = "";
        this->ip = "";
        this->port = 0;
    }

    void tell_impl(std::unique_ptr<message> msg) const;
    void ask_impl(std::unique_ptr<message> msg, const std::function<void(boost::any)> &response_fn) const;

};

}
