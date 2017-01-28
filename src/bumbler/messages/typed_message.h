#pragma once

#include <memory>
#include <boost/any.hpp>
#include "variant.h"
#include "message.h"
#include "../utility.h"

namespace bumbler {

BOOST_SERIALIZATION_ASSUME_ABSTRACT(message)

class typed_message : public message {
public:
    typed_message() { }

	typed_message(const typed_message& rhs);
	typed_message& operator=(const typed_message& rhs);

    typed_message(typed_message&&) = default; // support moving
    typed_message& operator=(typed_message&&) = default;

    ~typed_message() { }

    int get_priority() const override;

    bool is_broadcast() const override {
		return typeid(broadcast) == variant_->type();
    }

    bool is_stop_actor() const override {
		return typeid(stop_actor) == variant_->type();
    }

    bool is_kill_actor() const override {
		return typeid(kill_actor) == variant_->type();
    }

    bool is_priority_message() const override {
		return typeid(priority_message) == variant_->type();
    }

    boost::any get_data() const override;

    actor_ref get_sender() const override {
        return sender_;
    }

    actor_ref get_target() const override {
        return target_;
    }

    void set_sender(const actor_ref& sender) override {
        this->sender_ = sender;
    }

    void set_target(const actor_ref& target) override {
        this->target_ = target;
    }

    std::unique_ptr<message> clone() const override {
        return std::make_unique<typed_message>(*this);
    }

private:	
	actor_ref target_;
	actor_ref sender_;
	std::unique_ptr<variant> variant_;

	friend class typed_message_factory;
	typed_message(const actor_ref& target,
				  const actor_ref& sender,
				  std::unique_ptr<variant> variant);

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        boost::serialization::void_cast_register<typed_message, message>();

        ar & this->sender_;
        ar & this->target_;
        ar & this->variant_;
    }
};

class typed_message_factory {
public:
    static std::unique_ptr<typed_message> create(const actor_ref& target, const actor_ref& sender, std::unique_ptr<variant> variant) {
        return std::unique_ptr<typed_message>(new typed_message(target, sender, std::move(variant)));
    }
};

}
