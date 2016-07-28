#pragma once

#include <boost/any.hpp>
#include "untyped_actor.h"
#include "../internal/bumbler.h"

namespace bumbler {

class typed_promise_actor : public untyped_actor {
public:
    typed_promise_actor(const ResponseFun &response_fun_) : response_fun_(response_fun_) { }

protected:
    void on_receive(const boost::any& data) {
        response_fun_(data);
    }

private:
    ResponseFun response_fun_;
};

}
