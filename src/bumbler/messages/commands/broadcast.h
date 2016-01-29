#pragma once

#include "command.h"

namespace bumbler {

template<typename T>
class broadcast : public command<T> {
public:
    broadcast() { }
    broadcast(T data, int priority = 0) : command<T>(data, priority) {}
};

}
