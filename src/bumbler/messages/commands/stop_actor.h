#pragma once

#include "command.h"

namespace bumbler {

template<typename T>
class stop_actor : public command<T> {
public:
    stop_actor() {}
    stop_actor(T data, int priority = 0) : command<T>(data, priority) {}
};

}
