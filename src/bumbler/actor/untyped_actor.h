#pragma once

#include <memory>
#include <functional>
#include <boost/any.hpp>
#include "actor_ref/actor_ref.h"
#include "../messages/typed_message.h"
#include "../messages/commands/commands.h"

namespace bumbler {

class abstract_actor;

class untyped_actor {
public:
	virtual ~untyped_actor() { }

    virtual void on_receive(boost::any data) = 0;
    virtual void on_error(boost::any data, std::exception ex) { BOOST_LOG_TRIVIAL(error) << "[UNTYPED_ACTOR] " << ex.what(); };

    void set_reply_message_function(std::function<void(std::unique_ptr<message>)> func) {
        this->send_reply_message_func_ = std::move(func);
    }

    void set_sender(const actor_ref& sender) {
        this->sender_ = sender;
    }

    void set_self(const actor_ref& self) {
        this->self_ = self;
    }

protected:
    void reply(const char* data) {
        this->reply(std::string(data));
    }

    template<typename T>
    void reply(T&& data) {
        if(get_sender().is_none()) {
            BOOST_LOG_TRIVIAL(error) << "reply failed, sender is not set";
            return;
        }

        auto typed_msg = construct_reply_message(std::forward<T>(data));

        if(send_reply_message_func_) {
            send_reply_message_func_(std::move(typed_msg));
        }
    }

    template<typename T>
    T cast_message(boost::any& data) {
        if(data.type().hash_code() == typeid(T).hash_code()) {
            return boost::any_cast<T>(data);
        }
        else {
            throw std::runtime_error("Types mismatch in cast_message");
        }
    }

    template<typename T>
    bool is_type(boost::any& data) {
        return data.type().hash_code() == typeid(T).hash_code();
    }

    actor_ref get_self() {
        return self_;
    }

    actor_ref get_sender() {
        return sender_;
    }

private:
    actor_ref sender_;
    actor_ref self_;
    std::function<void(std::unique_ptr<message>)> send_reply_message_func_;

    template<typename T>
    std::unique_ptr<message> construct_reply_message(T&& data) {
        auto tm = typed_message_factory::create(get_sender(), get_self(), std::forward<T>(data));
        return std::move(tm);
    }
};

}
