#pragma once

#include "../actor/untyped_actor.h"

namespace bumbler {

class typed_promise_actor : public untyped_actor {
public:
    typed_promise_actor(const std::function<void(boost::any)> &response_fn_) : response_fn_(response_fn_) { }

protected:
    void on_receive(boost::any data) {
        response_fn_(data);
    }

private:
    std::function<void(boost::any)> response_fn_;
};

}
