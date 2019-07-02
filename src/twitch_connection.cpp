#include <iostream>
#include <asio/connect.hpp>
#include <asio/read_until.hpp>
#include <asio/streambuf.hpp>

#include "twitch_connection.h"

namespace gh {
	
	twitch_connection::twitch_connection(asio::io_context& io_context)
		: socket(io_context), buffer(1024)
	{
		using asio::ip::tcp;

		tcp::resolver resolver(io_context);
		tcp::resolver::results_type endpoints = resolver.resolve("irc.chat.twitch.tv", "6667");
		asio::connect(socket, endpoints);
	}
	
	void twitch_connection::authenticate(std::string const& username, std::string const& password)
	{
		auto pass_line = "PASS " + password + "\r\n";
		auto nick_line = "NICK " + username + "\r\n";
		socket.write_some(asio::buffer(pass_line));
		socket.write_some(asio::buffer(nick_line));
	}

	std::function<size_t(const std::string&)> twitch_connection::sendToChannel(const std::string& channel)
	{
		return [=](const std::string& message) -> size_t {
			auto msg = "PRIVMSG #" + channel + " :" + message + "\r\n";
			return socket.write_some(asio::buffer(msg));
		};
	}

	std::string twitch_connection::receive()
	{
		std::vector<char> data(1024);
		socket.read_some(asio::buffer(data));
		return std::string(data.begin(), data.end());
	}

	void twitch_connection::async_receive()
	{
		std::cout << "[START] async_receive\n";
		socket.async_read_some(asio::buffer(buffer),
			[=](const std::error_code& e, std::size_t size) {
				std::cout << "handler\n";
				std::string message(buffer.begin(), buffer.end());
				handler(message);
				async_receive();
			});
		std::cout << "[END] async_receive\n";
	}
	
	void twitch_connection::send(std::string const& message)
	{
		socket.write_some(asio::buffer(message));
	}
}
