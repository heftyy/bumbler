#pragma once

#include <vector>
#include "server_info.h"

class server_list
{
public:
	std::vector<server_info> servers;

	server_list() 
	{
		actor_system_ = std::shared_ptr<actor_system>(new actor_system("bumbler_system", 8558));

        //actor_system_->schedule("Abc", <#(message&)message#>, 1000, 5000);
	}
	
	~server_list()
	{
		
	}
	
	server_info& add_server(std::string address)
	{
		int port_start = address.find(':');
		std::string ip = address.substr(0, port_start);
		int port = atoi(address.substr(port_start + 1, address.length() - port_start - 1).c_str());
		
		return add_server_(ip, port);
	}
	
	server_info& add_server(std::string ip, int port)
	{
		return add_server_(ip, port);
	}
	
private:
	std::shared_ptr<actor_system> actor_system_;
	
	server_info& add_server_(std::string ip, int port)
	{
		server_info server_info(ip, port, actor_system_);
		servers.push_back(server_info);
		return servers.back();
	}
};

