#pragma once

#include <memory>

namespace bumbler {

class message;
class identifier;

namespace messaging_service {

void send_message(std::unique_ptr<message> msg);

}

}
