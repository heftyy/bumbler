#pragma once

#include <vector>
#include <array>
#include <sstream>
#include <memory>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class server_status
{
public:
	friend class boost::serialization::access;
	std::string name;
	std::string motd;
	int players_online;
	int players_max;
	int ping;

	server_status(std::string name, std::string motd, int players_online, int players_max, int ping) 
		: name(name), motd(motd), players_online(players_online), players_max(players_max), ping(ping) {}

	server_status(std::string received_data)
	{
		std::istringstream is(received_data);

		boost::archive::text_iarchive in_archive(is);
		server_status status;
		in_archive >> &status;
		std::memcpy(this, status, sizeof(status));
	}


	std::string get_serialized()
	{
		std::ostringstream archive_stream;
		boost::archive::text_oarchive out_archive(archive_stream);
		out_archive & this;
		return archive_stream.str();
	}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & players_max;
		ar & players_online;
		ar & ping;
		ar & name;
		ar & motd;
	}
};