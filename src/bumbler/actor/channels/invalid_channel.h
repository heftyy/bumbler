#pragma once

#include "abstract_channel.h"

namespace bumbler {

class invalid_channel : public abstract_channel {
public:
    invalid_channel() : abstract_channel() { }
};

}