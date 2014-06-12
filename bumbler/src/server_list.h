#pragma once

#include <vector>
#include "server_info.h"

class server_list
{
public:
	server_list() 
	{
		
	}
	
	~server_list()
	{
		
	}
	
	void add_server(std::string address) 
	{
		int port_start = address.find(':');
		std::string ip = address.substr(0, port_start - 1);
		int port = atoi(address.substr(port_start + 1, address.length() - port_start - 1).c_str());
		
		add_server_(ip, port);
	}
	
	void add_server(std::string ip, int port) 
	{
		add_server_(ip, port);
	}
	
private:
	std::vector<server_info> servers_;
	
	void add_server_(std::string ip, int port)
	{
		server_info sv_info(ip, port);
	}
};

