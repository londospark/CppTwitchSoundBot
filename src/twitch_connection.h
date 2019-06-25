#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace gh
{
	class twitch_connection
	{
	public:
		twitch_connection(boost::asio::io_context& io_context);
		void authenticate(std::string const& username, std::string const& password);
		std::function<size_t(const std::string&)> sendToChannel(const std::string& channel);
		std::string receive();
		void twitch_connection::async_receive(std::function<void(std::string)> handler);
		void send(std::string const& message);

		twitch_connection(twitch_connection&&) = default;
		twitch_connection(const twitch_connection&) = delete;
		twitch_connection& operator=(const twitch_connection&) = delete;

	private:
		boost::asio::ip::tcp::socket socket;
		std::vector<char> buffer;
	};
}

