#pragma once

#include "command.h"

namespace bumbler {

class stop_actor : public command {
public:
    stop_actor() {}
    template<typename T>
    stop_actor(const T& data) : command(data) {}
};

}
