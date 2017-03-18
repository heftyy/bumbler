#pragma once

#include <memory>
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "../actor/actor_ref/actor_ref.h"
#include "../messages/typed_message.h"

namespace bumbler {

enum packet_type {
    PACKET_CONNECT, PACKET_DISCONNECT, PACKET_COMMAND, PACKET_RESULT, PACKET_DATA
};

class packet_header {
public:
    packet_type type;

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & type;
    }
};

class packet_data {
public:
    std::string data;

    packet_data() { }

    explicit packet_data(const std::string& d) : data(d) { }
    explicit packet_data(const std::vector<char>& d) : data(d.begin(), d.end()) { }
    explicit packet_data(char *data, size_t length) : data(std::string(data, data + sizeof(char) * length)) { }

    void load(std::unique_ptr<message> msg);

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & data;
    }
};

}
