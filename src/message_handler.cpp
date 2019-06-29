#include "message_handler.h"

#include <iostream>
#include "twitch_message.h"

namespace gh {

	message_handler::message_handler(std::vector<gh::command> const& bot_commands,
		std::function<size_t(const std::string&)> const& send_message,
		gh::twitch_connection& twitch) : bot_commands(bot_commands), send_message(send_message), twitch(twitch) {};

	void message_handler::operator()(std::string const& reply)
	{
		if (auto message = gh::twitch_message::parse(reply))
		{
			std::cout << reply << std::flush;
			std::cout << "User: " << message->username << std::endl;
			std::cout << "channel: " << message->channel << " message: " << message->body << std::endl;

			if (auto user = message->user)
			{
				for (auto command_impl : bot_commands)
				{
					command_impl(message->body, *user, send_message);
				}
			}
		}
		else if (reply.find("PING :tmi.twitch.tv") != std::string::npos)
		{
			twitch.send("PONG :tmi.twitch.tv\r\n");
		}
		else {
			std::cout << "[UNRECOGNISED]: " << reply << std::endl;
		}
	};
}