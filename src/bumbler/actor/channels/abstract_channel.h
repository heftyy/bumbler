#pragma once

#include <functional>
#include <future>

#include "../../logger/logger.h"
#include "../../messages/typed_message.h"

namespace bumbler {

class abstract_channel {
public:
    void tell(std::unique_ptr<message> msg) {
        tell_impl(std::move(msg));
    }

    template<typename Result>
    std::future<Result> ask(std::unique_ptr<message> msg) {
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

        ask_impl(std::move(msg), future_func);

        return f;
    }

    virtual ~abstract_channel() = default;

protected:
    abstract_channel() { }

private:
    virtual void tell_impl(std::unique_ptr<message> msg) = 0;
    virtual void ask_impl(std::unique_ptr<message> msg, const std::function<void(boost::any)> &response_fn) = 0;
};

}