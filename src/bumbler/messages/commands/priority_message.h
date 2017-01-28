#pragma once

#include "command.h"

namespace bumbler {

class priority_message : public command {
public:
    priority_message() {}
	template<typename T>
    priority_message(const T& data, int priority) : command(data, priority) {}
};

}
