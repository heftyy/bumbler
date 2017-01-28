#pragma once

#include "command.h"

namespace bumbler {

class broadcast : public command {
public:
    broadcast() : command() { }
	template<typename T>
    broadcast(const T& data) : command(data) {}
};

}
