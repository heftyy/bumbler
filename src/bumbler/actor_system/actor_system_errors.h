#pragma once

#include <stdexcept>

enum {
    bumbler_ACTOR_NOT_FOUND = 1000,
    bumbler_ACTOR_ALREADY_EXISTS,
    bumbler_SERVER_DOESNT_EXIST,
    bumbler_WRONG_ACTOR_SYSTEM,
    bumbler_WRONG_ACTOR_METHOD,
    ACTOR_SYSTEM_STOPPED,
    ACTOR_SYSTEM_DOESNT_EXIST
};

/*
class actor_already_exists : public std::runtime_error {
public:
	actor_already_exists(const std::string& message) : std::runtime_error(message) { };
};

class actor_not_found : public std::runtime_error {
public:
	actor_not_found(const std::string& message) : std::runtime_error(message) { };
};

class server_doesnt_exist : public std::runtime_error {
public:
	server_doesnt_exist(const std::string& message) : std::runtime_error(message) { };
};

class wrong_actor_system: public std::runtime_error {
public:
	wrong_actor_system(const std::string& message) : std::runtime_error(message) { };
};

class wrong_actor_method: public std::runtime_error {
public:
	wrong_actor_method(const std::string& message) : std::runtime_error(message) { };
};
*/

static int bumbler_error(int error_code, const std::string& msg) {
    std::string error_name = "UNKNOWN ERROR";
    switch (error_code) {
        case bumbler_ACTOR_NOT_FOUND:
            error_name = "bumbler_ACTOR_NOT_FOUND";
            break;
        case bumbler_ACTOR_ALREADY_EXISTS:
            error_name = "bumbler_ACTOR_ALREADY_EXISTS";
            break;
        case bumbler_SERVER_DOESNT_EXIST:
            error_name = "bumbler_SERVER_DOESNT_EXIST";
            break;
        case bumbler_WRONG_ACTOR_SYSTEM:
            error_name = "bumbler_WRONG_ACTOR_SYSTEM";
            break;
        case bumbler_WRONG_ACTOR_METHOD:
            error_name = "bumbler_WRONG_ACTOR_METHOD";
            break;
        case ACTOR_SYSTEM_STOPPED:
            error_name = "ACTOR_SYSTEM_STOPPED";
            break;
        case ACTOR_SYSTEM_DOESNT_EXIST:
            error_name = "ACTOR_SYSTEM_DOESNT_EXIST";
    }
    BOOST_LOG_TRIVIAL(error) << error_code << ":" << error_name << " " << msg;
    return error_code;
}