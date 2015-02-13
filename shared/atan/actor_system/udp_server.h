#pragma once

#include <cstdlib>
#include <iostream>
#include <array>
#include <atomic>
#include <boost/asio.hpp>
#include "../packet/packet.h"

#define PACKET_MAX_LENGTH 4096

using boost::asio::ip::udp;

class udp_server
{
public:
	std::function<void(std::unique_ptr<packet>, udp::endpoint)> receive_function;
	std::function<void()> send_completed_function;

	udp_server(boost::asio::io_service& io_service, int port) 
		: port_(port), socket_(io_service, udp::endpoint(udp::v4(), (unsigned short)port)), future_socket_(io_service, udp::endpoint(udp::v4(), 0))
	{
		do_receive();
	}

	~udp_server()
	{
		stop();
	}

	void stop()
	{
		if (socket_.is_open())
		{
			socket_.close();
		}
		if (future_socket_.is_open())
		{
			future_socket_.close();
		}
	}

	std::unique_ptr<packet> future(const std::string& data, const udp::endpoint& target_endpoint, int timeout_ms)
	{
		sync_send(data, target_endpoint);
		std::unique_ptr<packet> packet;
		packet = sync_receive(target_endpoint, timeout_ms);
		if (packet)
		{
			return std::move(packet);
		}
		return nullptr;
	}

	void sync_send(const std::string& data, const udp::endpoint& target_endpoint)
	{
		future_socket_.send_to(boost::asio::buffer(data.c_str(), data.length()), target_endpoint);
	}

	std::unique_ptr<packet> sync_receive(const udp::endpoint& target_endpoint, int timeout_ms)
	{
		std::atomic<bool> stop_deadline(false);
		auto expires_at = std::chrono::system_clock::now() + std::chrono::milliseconds(1000);

		auto deadline_check = std::thread([&expires_at, &stop_deadline, this]()
		{
			while (!stop_deadline.load() && std::chrono::system_clock::now() < expires_at)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			if (!stop_deadline.load())
			{
				future_socket_.close();
				future_socket_.open(udp::v4());
			}
		});
		
		try
		{
			size_t bytes_recvd = future_socket_.receive_from(boost::asio::buffer(data_, PACKET_MAX_LENGTH), sender_endpoint_);
			if (bytes_recvd && sender_endpoint_.address().to_string().compare(target_endpoint.address().to_string()) == 0)
			{
				std::string string_data(data_.begin(), data_.begin() + bytes_recvd*sizeof(char));
				try
				{
					stop_deadline = true;
					deadline_check.join();
					auto received_packet = std::unique_ptr<packet>(new packet(string_data));
					return std::move(received_packet);
				}
				catch (std::exception e)
				{
					std::cerr << "[UDP_SERVER] parsing error on received packet" << std::endl;
				}
			}
		}
		catch (boost::system::system_error& ec)
		{
			std::cerr << "boost::system:error_code = " << ec.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "Got an exception!" << std::endl;
		}
		stop_deadline = true;
		deadline_check.join();
		return nullptr;
	}

	//void do_send(std::unique_ptr<std::vector<char>> data, udp::endpoint& target_endpoint)
	void do_send(const std::string& data, udp::endpoint& target_endpoint)
	{
		//std::cout << "do_send thread id = " << std::this_thread::get_id() << std::endl;
		socket_.async_send_to(
			boost::asio::buffer(data.c_str(), data.length()), target_endpoint,
			[this](boost::system::error_code, std::size_t)
		{
			send_completed_function();
		});
	}

	std::string get_socket_address()
	{
		std::string socket_address;
		socket_address.append(socket_.remote_endpoint().address().to_string());
		socket_address.append(":");
		//socket_address.append(socket_.remote_endpoint().port());
		std::cout << "Peer IP: " << socket_.remote_endpoint().address().to_string() << std::endl;
		return socket_address;
	}

private:
	udp::socket socket_;
	udp::socket future_socket_;
	udp::endpoint sender_endpoint_;
	std::array<char, PACKET_MAX_LENGTH> data_;
	int port_;

	void do_receive()
	{
		socket_.async_receive_from(
			boost::asio::buffer(data_, PACKET_MAX_LENGTH), sender_endpoint_,
			[this](boost::system::error_code ec, std::size_t bytes_recvd)
		{		
			if (!ec && bytes_recvd > 0)
			{
				std::cout << "do_receive thread id = " << std::this_thread::get_id() << " received bytes " << bytes_recvd << std::endl;
				std::string string_data(data_.begin(), data_.begin() + bytes_recvd*sizeof(char));
				try
				{
					auto received_packet = std::unique_ptr<packet>(new packet(string_data));
					receive_function(std::move(received_packet), sender_endpoint_);
				}
				catch (std::exception e)
				{
					std::cerr << "parsing error on received packet" << std::endl;
				}
			}
			do_receive();
		});
	}
};
