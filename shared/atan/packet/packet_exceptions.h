#pragma once

#include <stdexcept>

class packet_parsing_error : public std::runtime_error {
public:
	packet_parsing_error(const std::string& message) : std::runtime_error(message) { };
};

class packet_structure_error : public std::runtime_error {
public:
	packet_structure_error(const std::string& message) : std::runtime_error(message) { };
};
