#pragma once

#include <functional>
#include <future>
#include <boost/any.hpp>

#include "../../internal/bumbler.h"
#include "../../logger/logger.h"
#include "../../messages/message.h"

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

        ResponseFun future_func =
                [future_type_hashcode, promise_ptr](const boost::any& response) {
                    if (response.type().hash_code() == future_type_hashcode) {
                        Result value = boost::any_cast<Result>(response);
                        promise_ptr->set_value(value);
                    }
                    else {
                        BOOST_LOG_TRIVIAL(error) << "[FUTURE] expected type didn't match the replied type";
                        promise_ptr->set_value(Result());
                    }
                };

        ask_impl(std::move(msg), future_func);

        return f;
    }

    virtual bool expired() = 0;

    virtual ~abstract_channel() = default;

protected:
    abstract_channel() {}

private:
    virtual void tell_impl(std::unique_ptr<message> msg) = 0;
    virtual void ask_impl(std::unique_ptr<message> msg, const ResponseFun& response_fn) = 0;
};

}