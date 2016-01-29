#pragma once

#include "command.h"

namespace bumbler {

template<typename T>
class priority_message : public command<T> {
public:
    priority_message() {}
    priority_message(T data, int priority = 0) : command<T>(data, priority) {}
};

}
