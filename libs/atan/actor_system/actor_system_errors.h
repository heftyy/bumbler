#pragma once

#include <stdexcept>

enum
{
	ATAN_ACTOR_NOT_FOUND = 1000, ATAN_ACTOR_ALREADY_EXISTS, ATAN_SERVER_DOESNT_EXIST, ATAN_WRONG_ACTOR_SYSTEM, ATAN_WRONG_ACTOR_METHOD, ACTOR_SYSTEM_STOPPED
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

static int atan_error(int error_code, const std::string& msg)
{
	std::string error_name = "UNKNOWN ERROR";
	switch (error_code)
	{
	case ATAN_ACTOR_NOT_FOUND:
		error_name = "ATAN_ACTOR_NOT_FOUND";
		break;
	case ATAN_ACTOR_ALREADY_EXISTS:
		error_name = "ATAN_ACTOR_ALREADY_EXISTS";
		break;
	case ATAN_SERVER_DOESNT_EXIST:
		error_name = "ATAN_SERVER_DOESNT_EXIST";
		break;
	case ATAN_WRONG_ACTOR_SYSTEM:
		error_name = "ATAN_WRONG_ACTOR_SYSTEM";
		break;
	case ATAN_WRONG_ACTOR_METHOD:
		error_name = "ATAN_WRONG_ACTOR_METHOD";
		break;
	case ACTOR_SYSTEM_STOPPED:
		error_name = "ACTOR_SYSTEM_STOPPED";
		break;
	}
	std::cerr << error_code << ":" << error_name << " " << msg << std::endl;
	return error_code;
}