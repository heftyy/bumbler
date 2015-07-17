#pragma once

#include <atan/messages/typed_message.h>
#include <atan/messages/commands/commands.h>

BOOST_CLASS_EXPORT(typed_message<std::string>)
BOOST_CLASS_EXPORT(typed_message<double>)
BOOST_CLASS_EXPORT(typed_message<float>)
BOOST_CLASS_EXPORT(typed_message<long>)
BOOST_CLASS_EXPORT(typed_message<int>)

BOOST_CLASS_EXPORT(broadcast<std::string>)
BOOST_CLASS_EXPORT(broadcast<double>)
BOOST_CLASS_EXPORT(broadcast<float>)
BOOST_CLASS_EXPORT(broadcast<long>)
BOOST_CLASS_EXPORT(broadcast<int>)

BOOST_CLASS_EXPORT(stop_actor<std::string>)
BOOST_CLASS_EXPORT(stop_actor<double>)
BOOST_CLASS_EXPORT(stop_actor<float>)
BOOST_CLASS_EXPORT(stop_actor<long>)
BOOST_CLASS_EXPORT(stop_actor<int>)

BOOST_CLASS_EXPORT(kill_actor<std::string>)
BOOST_CLASS_EXPORT(kill_actor<double>)
BOOST_CLASS_EXPORT(kill_actor<float>)
BOOST_CLASS_EXPORT(kill_actor<long>)
BOOST_CLASS_EXPORT(kill_actor<int>)