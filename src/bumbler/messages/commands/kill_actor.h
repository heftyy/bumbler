#pragma once

#include "command.h"

namespace bumbler {

template<typename T>
class kill_actor : public command<T> {
public:
    kill_actor() {}
    kill_actor(T data, int priority = 0) : command<T>(data, priority) {}
};

}
