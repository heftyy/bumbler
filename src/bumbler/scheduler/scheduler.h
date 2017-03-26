#pragma once

#include <memory>

namespace bumbler {

class cancellable;
class message;

class scheduler {
public:
    std::shared_ptr<cancellable> schedule(std::unique_ptr<message> msg, long initial_delay_ms, long interval_ms) const;
};

}
