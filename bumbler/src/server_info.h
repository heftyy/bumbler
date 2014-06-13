#pragma once

#include <atan/actor/remote_actor.h>
#include <memory>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include "server_connection_actor.h"
#include "server_status.h"

class server_info
{
public:
	server_info(std::string ip, int port, actor_system& actor_system);
	
	~server_info()
	{
	}

	std::string to_string()
	{
		return ip_ + ":" + std::to_string(port_) + ", " + server_status_.to_string();
	}
			
private:
	std::string ip_;
	int port_;
	std::shared_ptr<remote_actor> server_connection_actor_;
	server_status server_status_;
	
	void get_server_status()
	{
		
	}
};