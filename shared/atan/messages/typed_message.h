#pragma once

#include <vector>
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/any.hpp>
#include "message.h"
#include "commands/commands.h"
#include "utility.h"

enum class message_type {
    regular, broadcast, stop_actor, kill_actor
};

template<typename T>
class typed_message : public message {
public:
    T data;
    std::unique_ptr<actor_ref> target;
    std::unique_ptr<actor_ref> sender;

    typed_message() { }

    typed_message(const typed_message<T>& msg) : message(msg) {
        std::cout << "typed-message copy ctor\n";
        this->data = msg.data;
        this->target = utility::make_unique<actor_ref>(*msg.target);
        this->sender = utility::make_unique<actor_ref>(*msg.sender);
        this->message_type_ = msg.message_type_;
    };

    typed_message(const actor_ref& target, const actor_ref& sender, const T& data, message_type msg_type = message_type::regular) {
        this->data = data;
        this->target = utility::make_unique<actor_ref>(std::move(target));
        this->sender = utility::make_unique<actor_ref>(std::move(sender));
        this->message_type_ = msg_type;
    }

    typed_message(typed_message<T>&&) = default; // support moving
    typed_message& operator=(typed_message<T>&&) = default;

    ~typed_message() {
        this->target.reset();
        this->sender.reset();
    }

    bool is_broadcast() const override {
        return message_type_ == message_type::broadcast;
    }

    bool is_stop_actor() const override {
        return message_type_ == message_type::stop_actor;
    }

    bool is_kill_actor() const override {
        return message_type_ == message_type::kill_actor;
    }

    boost::any get_data() const override {
        return data;
    }

    actor_ref& get_sender() const override {
        return *sender.get();
    }

    actor_ref& get_target() const override {
        return *target.get();
    }

    void set_sender(const actor_ref& sender) override {
        this->sender = utility::make_unique<actor_ref>(sender);
    }

    void set_target(const actor_ref& target) override {
        this->target = utility::make_unique<actor_ref>(target);
    }

    std::unique_ptr<message> clone() const override {
        auto tm = *this;
        auto result = utility::make_unique<decltype(tm)>(std::move(tm));

        return std::move(result);
    }

private:
    message_type message_type_ = message_type::regular;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        boost::serialization::void_cast_register<typed_message, message>();

        ar & this->sender;
        ar & this->target;
        ar & this->data;
        ar & this->message_type_;
    }
};

class typed_message_factory {
public:
    template<typename T>
    static auto create(const actor_ref& target, const actor_ref& sender, const T& data) -> const typed_message<T> {
        return typed_message<T>(target, sender, data);
    }

    template<typename T>
    static auto create(const actor_ref& target, const actor_ref& sender, const broadcast<T>& cmd) -> const typed_message<T> {
        return typed_message<T>(target, sender, cmd.data, message_type::broadcast);
    }

    template<typename T>
    static auto create(const actor_ref& target, const actor_ref& sender, const stop_actor<T>& cmd) -> const typed_message<T> {
        return typed_message<T>(target, sender, cmd.data, message_type::stop_actor);
    }

    template<typename T>
    static auto create(const actor_ref& target, const actor_ref& sender, const kill_actor<T>& cmd) -> const typed_message<T> {
        return typed_message<T>(target, sender, cmd.data, message_type::kill_actor);
    }
};