#pragma once

#include <memory>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "packet_structs.h"
#include "packet_exceptions.h"

namespace bumbler {

class packet {
public:
    friend class boost::serialization::access;

    packet_header header;
    packet_data data;

    packet() {
    }

    ~packet() {
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & header;
        ar & data;
    }

    packet(packet_header& h, packet_data& d) : header(h), data(d) {
    }

    packet(std::unique_ptr<message> msg) {
        header.type = PACKET_DATA;
        data.load(std::move(msg));
    }

    std::string get_serialized() {
        if (header.type == 0) {
            throw new packet_structure_error("packet had no type");
        }
        std::ostringstream archive_stream;
        boost::archive::text_oarchive out_archive(archive_stream);
        out_archive << *this;
        return archive_stream.str();
        //serialized_data = archive_stream.str();
        //return std::unique_ptr<std::vector<char>>(new std::vector<char>(serialized_data.begin(), serialized_data.end()));
    }

    static packet parse(const std::string& received_data) {
        std::istringstream is(received_data);

        boost::archive::text_iarchive in_archive(is);
        packet received_packet;
        in_archive >> received_packet;
        return received_packet;
    }

};

}
