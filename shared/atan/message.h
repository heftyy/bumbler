#pragma once

#include <vector>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/any.hpp>
#include "actor/actor_ref.h"

class message_interface
{
public:
    virtual bool is_valid() {}
    virtual bool send() {}
    virtual boost::any get_data() {}

    virtual actor_ref get_sender() = 0;
    virtual actor_ref get_target() = 0;
};

template<typename T>
class message : public message_interface
{
public:
	friend class boost::serialization::access;
	int type;
	actor_ref target;
	actor_ref sender;
	T data;
	//std::vector<char> data;

	message() : type(-1) {}
    /*
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
    */
    message(const actor_ref target, const actor_ref sender, T d, int type) : target(target), sender(sender), data(d), type(type) {}
    message(std::unique_ptr<actor_ref> target, std::unique_ptr<actor_ref> sender, T d, int type) : data(d), type(type)
    {
        if (target) this->target = *target.release();
        if (sender) this->sender = *sender.release();
    }

    boost::any get_data()
    {
        return data;
    }

	bool is_valid()
	{
        return type >= 0;
	}

    bool send()
    {
        if(target.exists())
            target.tell<T>(data, sender);
    }

    actor_ref get_sender()
    {
        return sender;
    }

    actor_ref get_target()
    {
        return target;
    }

	//static void restore_message(message& msg, std::vector<char>& received_data)
	static void restore_message(message<T>& msg, std::string& received_data)
	{
		std::string serialized_data(received_data.begin(), received_data.end());
		std::istringstream is(serialized_data);
		boost::archive::text_iarchive in_archive(is);
		in_archive >> msg;
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