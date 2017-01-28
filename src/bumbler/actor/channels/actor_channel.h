#pragma once

#include "abstract_channel.h"

namespace bumbler {

class actor_channel : public abstract_channel {
protected:
    actor_channel() : abstract_channel() { }
};

}
