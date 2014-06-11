#pragma once

#include <memory>
#include <iostream>

class actor_ref
{
public:
	std::string actor_name;
	std::string system_name;
	std::string ip;
	int port;

	actor_ref() : port(0) {}
	actor_ref(const std::string& actor_name, const std::string& system_name) : actor_name(actor_name), system_name(system_name), port(0) {}
	actor_ref(const std::string& actor_ref) /**[actor]$[system_name]@[ip]:[port]*/
	{
		int system_name_start = actor_ref.find('$');
		int ip_start = actor_ref.find('@');
		int port_start = actor_ref.find(':');
		actor_name = actor_ref.substr(0, system_name_start);
		system_name = actor_ref.substr(system_name_start + 1, ip_start - system_name_start - 1);
		ip = actor_ref.substr(ip_start + 1, port_start - ip_start - 1);
		port = atoi(actor_ref.substr(port_start + 1, actor_ref.length() - port_start - 1).c_str());
	}

	bool exists()
	{
		if (actor_name.length() > 0 && system_name.length() > 0) return true;
		return false;
	}

	bool valid_address()
	{
		if (actor_name.length() > 0 && system_name.length() > 0 && ip.length() > 0 && port > 0) return true;
		return false;
	}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & actor_name;
		ar & system_name;
		ar & ip;
		ar & port;
	}
};