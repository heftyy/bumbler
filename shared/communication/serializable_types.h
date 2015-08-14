#pragma once

#include <atan/messages/typed_message.h>
#include <atan/messages/commands/commands.h>

BOOST_CLASS_EXPORT(typed_message<std::string>)
BOOST_CLASS_EXPORT(typed_message<double>)
BOOST_CLASS_EXPORT(typed_message<float>)
BOOST_CLASS_EXPORT(typed_message<long>)
BOOST_CLASS_EXPORT(typed_message<int>)

BOOST_CLASS_EXPORT(typed_message<broadcast<std::string>>)
BOOST_CLASS_EXPORT(typed_message<broadcast<double>>)
BOOST_CLASS_EXPORT(typed_message<broadcast<float>>)
BOOST_CLASS_EXPORT(typed_message<broadcast<long>>)
BOOST_CLASS_EXPORT(typed_message<broadcast<int>>)

BOOST_CLASS_EXPORT(typed_message<stop_actor<std::string>>)
BOOST_CLASS_EXPORT(typed_message<stop_actor<double>>)
BOOST_CLASS_EXPORT(typed_message<stop_actor<float>>)
BOOST_CLASS_EXPORT(typed_message<stop_actor<long>>)
BOOST_CLASS_EXPORT(typed_message<stop_actor<int>>)

BOOST_CLASS_EXPORT(typed_message<kill_actor<std::string>>)
BOOST_CLASS_EXPORT(typed_message<kill_actor<double>>)
BOOST_CLASS_EXPORT(typed_message<kill_actor<float>>)
BOOST_CLASS_EXPORT(typed_message<kill_actor<long>>)
BOOST_CLASS_EXPORT(typed_message<kill_actor<int>>)