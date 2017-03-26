#pragma once

#include <cstdlib>
#include <iostream>
#include <array>
#include <atomic>
#include <boost/asio.hpp>
#include "../packet/packet.h"
#include "../logger/logger.h"

#define PACKET_MAX_LENGTH 4096

namespace bumbler {

using boost::asio::ip::udp;

class udp_server {
public:
    std::function<void(const packet&, const udp::endpoint&)> receive_function;
    std::function<void()> send_completed_function;

    udp_server(boost::asio::io_service& io_service, int port)
        : socket_(io_service, udp::endpoint(udp::v4(), static_cast<unsigned short>(port)))
        , port_(port) {
        do_receive();
    }

    ~udp_server() {
        stop();
    }

    void stop() {
        if (socket_.is_open()) {
            socket_.close();
        }
    }

    void do_send(const std::string& data, const udp::endpoint& target_endpoint) {
        socket_.async_send_to(
                boost::asio::buffer(data.c_str(), data.length()), target_endpoint,
                [this](boost::system::error_code, std::size_t) {
                    send_completed_function();
                });
    }

    std::string get_socket_address() {
        std::string socket_address;
        socket_address.append(socket_.remote_endpoint().address().to_string());
        socket_address.append(":");
        //socket_address.append(socket_.remote_endpoint().port());
        BOOST_LOG_TRIVIAL(debug) << "Peer IP: " << socket_.remote_endpoint().address().to_string();
        return socket_address;
    }

private:
    udp::socket socket_;
    udp::endpoint sender_endpoint_;
    std::array<char, PACKET_MAX_LENGTH> data_;
    int port_;

    void do_receive() {
        socket_.async_receive_from(
                boost::asio::buffer(data_, PACKET_MAX_LENGTH), sender_endpoint_,
                [this](boost::system::error_code ec, std::size_t bytes_recvd) {
                    if (!ec && bytes_recvd > 0) {
                        std::string string_data(data_.begin(), data_.begin() + bytes_recvd * sizeof(char));
                        try {
                            packet received_packet = packet::parse(string_data);
                            receive_function(received_packet, sender_endpoint_);
                        }
                        catch (std::exception e) {
                            BOOST_LOG_TRIVIAL(error) << "parsing error on received packet";
                        }
                    }
                    do_receive();
                });
    }
};

}
