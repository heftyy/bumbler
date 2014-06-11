#pragma once

#include <memory>
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "../message.h"

enum packet_type
{
	PACKET_CONNECT, PACKET_DISCONNECT, PACKET_COMMAND, PACKET_RESULT, PACKET_DATA
};

class packet_header
{
public:
	friend class boost::serialization::access;
	packet_type type;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & type;
	}
};

class packet_data
{
public:
	friend class boost::serialization::access;
	//std::vector<char> data;
	std::string data;

	packet_data() {}
	packet_data(const std::string& d) : data(d) {}
	packet_data(const std::vector<char>& d) : data(d.begin(), d.end()) {}
	packet_data(char *data, size_t length) : data(std::string(data, data + sizeof(char)*length)) {}
	packet_data(message& msg)
	{
		//std::ostringstream archive_stream;
		//boost::archive::text_oarchive archive(archive_stream);
		//archive << msg;
		//std::string result = std::string(archive_stream.str());
		//data = result;

		data = msg.get_raw_message();

		//data = std::vector<char>(result.begin(), result.end());
	}
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & data;
	}
};