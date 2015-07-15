#pragma once

#include <atan/messages/typed_message.h>
#include <atan/messages/broadcast.h>

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