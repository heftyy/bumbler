#pragma once

#include <functional>

#include "../actor_ref/actor_ref.h"
#include "../abstract_actor.h"
#include "../../messages/typed_message.h"

namespace bumbler {

class abstract_channel {
public:
    //if a string literal is passed to tell change it to std::string
    void tell(char* data, actor_ref sender = actor_ref::none()) {
        this->tell(std::string(data), sender);
    }

    template<typename T>
    void tell(T&& data, actor_ref sender = actor_ref::none()) const {
        auto tm = typed_message_factory::create(actor_ref_, sender, std::forward<T>(data));
        tell_impl(std::move(tm));
    }

    template<typename T>
    void tell(const typed_message<T>& msg) const {
        tell_impl(utility::make_unique<typed_message<T>>(std::move(msg)));
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

        auto tm = typed_message_factory::create(actor_ref_, actor_ref::none(), std::forward<T>(data));

        ask_impl(std::move(tm), future_func);

        return f;
    }

protected:
    abstract_channel() { }

    abstract_channel(const actor_ref& actor_ref_, const std::shared_ptr<abstract_actor>& actor_ptr_) :
            actor_ref_(actor_ref_), actor_ptr_(actor_ptr_) { }

private:
    actor_ref actor_ref_;
    std::shared_ptr<abstract_actor> actor_ptr_;

    virtual void tell_impl(std::unique_ptr<message> msg) const = 0;
    virtual void ask_impl(std::unique_ptr<message> msg, const std::function<void(boost::any)> &response_fn) const = 0;
};

}