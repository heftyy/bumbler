#pragma once

#include <vector>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "actor/actor_ref.h"

class message
{
public:
	friend class boost::serialization::access;
	int type;
	actor_ref target;
	actor_ref sender;
	std::string data;
	//std::vector<char> data;

	message() : type(-1) {}
	message(const actor_ref target, const actor_ref sender, const std::vector<char> d, int type) : target(target), sender(sender), data(d.begin(), d.end()), type(type) {}
	message(std::unique_ptr<actor_ref> target, std::unique_ptr<actor_ref> sender, const std::vector<char>& d, int type) : data(d.begin(), d.end()), type(type)
	{
		if (target) this->target = *target.release();
		if (sender) this->sender = *sender.release();
	}
	message(const actor_ref target, const actor_ref sender, const std::string d, int type) : target(target), sender(sender), data(d), type(type) {}
	message(std::unique_ptr<actor_ref> target, std::unique_ptr<actor_ref> sender, const std::string d, int type) : data(d), type(type)
	{
		if (target) this->target = *target.release();
		if (sender) this->sender = *sender.release();
	}

	bool is_valid()
	{
		if (type < 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	//static void restore_message(message& msg, std::vector<char>& received_data)
	static void restore_message(message& msg, std::string& received_data)
	{
		std::string serialized_data(received_data.begin(), received_data.end());
		std::istringstream is(serialized_data);
		boost::archive::text_iarchive in_archive(is);
		in_archive >> msg;
	}

	std::string get_raw_message()
	{
		std::ostringstream archive_stream;
		boost::archive::text_oarchive out_archive(archive_stream);
		out_archive << this;
		std::string serialized_data;
		return archive_stream.str();
	}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & type;
		ar & sender;
		ar & target;
		ar & data;
	}
};