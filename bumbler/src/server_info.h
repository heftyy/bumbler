#pragma once

#include <atan/actor/remote_actor.h>
#include <memory>
#include "server_connection_actor.h"
#include "server_status.h"

class server_info
{
public:
	server_info(std::string ip, int port, actor_system& actor_system) : port_(port), ip_(ip) 
	{
		std::string rpi_actor_ref = "server_actor$bumbler_server@" + ip + ":" + std::to_string(port);
		server_connection_actor_ = std::shared_ptr<server_connection_actor>(new server_connection_actor(rpi_actor_ref, actor_system));
		actor_system.add_actor(server_connection_actor_);
	}
	
	~server_info()
	{
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
