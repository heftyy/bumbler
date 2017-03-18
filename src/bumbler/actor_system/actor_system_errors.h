#pragma once

#include <stdexcept>

namespace bumbler {

class bumbler_error : public std::runtime_error {
public:
    bumbler_error(const std::string& message) : std::runtime_error(message) { };
};

class actor_already_exists : public bumbler_error {
public:
    actor_already_exists(const std::string& message) : bumbler_error(message) { };
};

class actor_not_found : public bumbler_error {
public:
    actor_not_found(const std::string& message) : bumbler_error(message) { };
};

class actor_system_stopped : public bumbler_error {
public:
    actor_system_stopped(const std::string& message) : bumbler_error(message) { };
};

class server_doesnt_exist : public bumbler_error {
public:
    server_doesnt_exist(const std::string& message) : bumbler_error(message) { };
};

class wrong_actor_system: public bumbler_error {
public:
    wrong_actor_system(const std::string& message) : bumbler_error(message) { };
};

class wrong_actor_method: public bumbler_error {
public:
    wrong_actor_method(const std::string& message) : bumbler_error(message) { };
};


}
