#pragma once

#include <bumbler/messages/typed_message.h>
#include <bumbler/messages/typed_variant.h>
#include <bumbler/messages/commands/commands.h>

using namespace bumbler;

BOOST_CLASS_EXPORT(typed_message)

BOOST_CLASS_EXPORT(typed_variant<std::string>)
BOOST_CLASS_EXPORT(typed_variant<double>)
BOOST_CLASS_EXPORT(typed_variant<float>)
BOOST_CLASS_EXPORT(typed_variant<long>)
BOOST_CLASS_EXPORT(typed_variant<int>)

BOOST_CLASS_EXPORT(typed_variant<broadcast>)
BOOST_CLASS_EXPORT(typed_variant<stop_actor>)
BOOST_CLASS_EXPORT(typed_variant<kill_actor>)
BOOST_CLASS_EXPORT(typed_variant<priority_message>)