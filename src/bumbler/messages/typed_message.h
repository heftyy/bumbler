#pragma once

#include <vector>
#include <sstream>
#include <memory>
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
#include "../utility.h"

namespace bumbler {

BOOST_SERIALIZATION_ASSUME_ABSTRACT(message)

enum class message_type {
    regular,
    broadcast,
    stop_actor,
    kill_actor,
    priority_message
};

template<typename T>
class typed_message : public message {
public:
    T data;
    std::unique_ptr<actor_ref> target;
    std::unique_ptr<actor_ref> sender;
    int priority;

    typed_message() { }

    typed_message(const typed_message& rhs) : message(rhs) {
        std::cout << "typed_message copy ctor\n";
        this->data = rhs.data;
        this->target = std::make_unique<actor_ref>(*rhs.target);
        this->sender = std::make_unique<actor_ref>(*rhs.sender);
        this->message_type_ = rhs.message_type_;
        this->priority = rhs.priority;
    }

	typed_message& operator=(const typed_message& rhs) {
		std::cout << "typed_message assign ctor\n";
		this->data = rhs.data;
		this->target = std::make_unique<actor_ref>(*rhs.target);
		this->sender = std::make_unique<actor_ref>(*rhs.sender);
		this->message_type_ = rhs.message_type_;
        this->priority = rhs.priority;
		return *this;
	}

    typed_message(const actor_ref& target,
                  const actor_ref& sender,
                  const T& data,
                  message_type msg_type = message_type::regular,
                  int priority = 0) {
        this->data = data;
        this->target = std::make_unique<actor_ref>(target);
        this->sender = std::make_unique<actor_ref>(sender);
        this->message_type_ = msg_type;
        this->priority = priority;
    }

    typed_message(typed_message<T>&&) = default; // support moving
    typed_message& operator=(typed_message<T>&&) = default;

    ~typed_message() {
        this->target.reset();
        this->sender.reset();
    }

    int get_priority() const override {
        return priority;
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

    bool is_priority_message() const override {
        return message_type_ == message_type::priority_message;
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
        this->sender = std::make_unique<actor_ref>(sender);
    }

    void set_target(const actor_ref& target) override {
        this->target = std::make_unique<actor_ref>(target);
    }

    std::unique_ptr<message> clone() const override {
        return std::unique_ptr<message>(new typed_message(*this));
    }

    std::unique_ptr<typed_message<T>> clone_typed() const {
        return std::unique_ptr<typed_message<T>>(new typed_message(*this));
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
    static auto create(const actor_ref& target, const actor_ref& sender, const T& data) -> std::unique_ptr<typed_message<T>> {
        return std::make_unique<typed_message<T>>(target, sender, data);
    }

    template<typename T>
    static auto create(const actor_ref& target, const actor_ref& sender, const broadcast<T>& cmd) -> std::unique_ptr<typed_message<T>> {
        return std::make_unique<typed_message<T>>(target, sender, cmd.data, message_type::broadcast);
    }

    template<typename T>
    static auto create(const actor_ref& target, const actor_ref& sender, const stop_actor<T>& cmd) -> std::unique_ptr<typed_message<T>> {
        return std::make_unique<typed_message<T>>(target, sender, cmd.data, message_type::stop_actor);
    }

    template<typename T>
    static auto create(const actor_ref& target, const actor_ref& sender, const kill_actor<T>& cmd) -> std::unique_ptr<typed_message<T>> {
        return std::make_unique<typed_message<T>>(target, sender, cmd.data, message_type::kill_actor);
    }

    template<typename T>
    static auto create(const actor_ref& target, const actor_ref& sender, const priority_message<T>& cmd) -> std::unique_ptr<typed_message<T>> {
        return std::make_unique<typed_message<T>>(target, sender, cmd.data, message_type::priority_message, cmd.priority);
    }
};

}
