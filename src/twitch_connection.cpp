#include <boost/asio/connect.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>

#include "twitch_connection.h"

namespace gh {
	
	twitch_connection::twitch_connection(boost::asio::io_context& io_context)
		: socket(io_context), buffer(1024)
	{
		using boost::asio::ip::tcp;

		tcp::resolver resolver(io_context);
		tcp::resolver::results_type endpoints = resolver.resolve("irc.chat.twitch.tv", "6667");
		boost::asio::connect(socket, endpoints);
	}
	
	void twitch_connection::authenticate(std::string const& username, std::string const& password)
	{
		auto pass_line = "PASS " + password + "\r\n";
		auto nick_line = "NICK " + username + "\r\n";
		socket.write_some(boost::asio::buffer(pass_line));
		socket.write_some(boost::asio::buffer(nick_line));
	}

	std::function<size_t(const std::string&)> twitch_connection::sendToChannel(const std::string& channel)
	{
		return [=](const std::string& message) -> size_t {
			auto msg = "PRIVMSG #" + channel + " :" + message + "\r\n";
			return socket.write_some(boost::asio::buffer(msg));
		};
	}

	std::string twitch_connection::receive()
	{
		std::vector<char> data(1024);
		socket.read_some(boost::asio::buffer(data));
		return std::string(data.begin(), data.end());
	}

	void twitch_connection::async_receive(std::function<void(std::string)> handler)
	{
		std::cout << "async_receive\n";
		socket.async_read_some(boost::asio::buffer(buffer),
			[=](const boost::system::error_code& e, std::size_t size) {
				std::cout << "handler\n";
				std::string message(buffer.begin(), buffer.end());
				handler(message);
				async_receive(handler);
			});
		std::cout << "[END] async_receive\n";
	}
	
	void twitch_connection::send(std::string const& message)
	{
		socket.write_some(boost::asio::buffer(message));
	}
}