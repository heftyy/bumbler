#pragma once

#include <vector>
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/any.hpp>
#include "message.h"
#include "commands/commands.h"

template<typename T>
class typed_message : public message {
public:
    std::shared_ptr<actor_ref> target;
    std::shared_ptr<actor_ref> sender;
    T data;

    typed_message() { }

	typed_message(const typed_message<T>& msg) : message(msg) {
		this->data = msg.data;
		this->target = msg.target;
		this->sender = msg.sender;;
    };

    typed_message(const actor_ref& target, const actor_ref& sender, const T& data) {
        this->set_target(target);
        this->set_sender(sender);
        this->data = data;
    }

    typed_message(const std::shared_ptr<actor_ref>& target, const std::shared_ptr<actor_ref>& sender, const T& data) {
        this->target = target;
        this->sender = sender;
        this->data = data;
    }

	~typed_message() {
		this->target.reset();
		this->sender.reset();		
    }

    virtual bool is_broadcast() const override {
        return std::is_base_of<::untyped_broadcast, T>::value;
    }

    virtual bool is_stop_actor() const override {
        return std::is_base_of<::untyped_stop_actor, T>::value;
    }

    virtual bool is_kill_actor() const override {
        return std::is_base_of<::untyped_kill_actor, T>::value;
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
        this->sender = std::make_shared<actor_ref>(sender);
    }

    void set_target(const actor_ref& target) override {
        this->target = std::make_shared<actor_ref>(target);
    }

    std::unique_ptr<message> clone() const override {
        auto result = std::unique_ptr<typed_message<T>>(new typed_message<T>(this->get_target(), this->get_sender(), static_cast<T>(this->data)));

        return std::move(result);
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        boost::serialization::void_cast_register<typed_message, message>();

        ar & this->sender;
        ar & this->target;
        ar & this->data;
    }
};