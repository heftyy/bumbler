#pragma once

#include "command.h"

namespace bumbler {

class kill_actor : public command {
public:
    kill_actor() {}
	template<typename T>
    kill_actor(const T& data) : command(data) {}
};

}
