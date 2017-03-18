#pragma once

#include <functional>

namespace boost { class any; }

namespace bumbler {

enum class stop_mode {
	IGNORE_QUEUE, WAIT_FOR_QUEUE
};

using ResponseFun = std::function<void(const boost::any&)>;

}

