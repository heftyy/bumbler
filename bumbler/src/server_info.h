#pragma once

#Include <atan/actor/remote_actor.h>
#include <memory>
#include "server_connection_actor.h"

class server_info
{
public:
	server_info(std::string ip, int port, actor_system& actor_system) : port_(port), ip_(ip) 
	{
		std::string rpi_actor_ref = "server_actor$bumbler_server@" + ip + ":" + port;
		server_connection_actor_ = std::shared_ptr<server_connection_actor>(new server_connection_actor(rpi_actor_ref, actor_system));
	}
	
	~server_info()
	{
	}
			
private:
	std::string ip_;
	int port_;
	std::shared_ptr<remote_actor> server_connection_actor_;
};
