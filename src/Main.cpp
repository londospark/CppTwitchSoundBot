#include "pch.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <map>
#include <sstream>
#include <codecvt>
#include <optional>

#include "twitch_message.h"
#include "bot_command.h"
#include "command_registry.h"
#include "command_definitions.h"
#include "horn_command.h"
#include "twitch_connection.h"

int
main()
{
	std::ifstream file("supersecret.txt");
	std::string oauth_token;

	if (file.is_open())
	{
		std::getline(file, oauth_token, '\n');
		file.close();
	}
	boost::asio::io_context io_context;
	gh::twitch_connection twitch(io_context);
	twitch.authenticate("hubballbot", oauth_token);

	std::cout << twitch.receive() << std::endl;

	twitch.send("JOIN #garethhubball\r\n");
	twitch.send("CAP REQ :twitch.tv/tags\r\n");

	auto const send_message = twitch.sendToChannel("garethhubball");
	
	/*if (result != 0)
	{
		std::cout << "Database not opened." << std::endl;
		return result;
	}*/

	gh::command_registry registry("hubballbot.db");	
	registry.load_commands(); //TODO(gareth): Check that we've not had a problem here.

	gh::horn_command horn(30);

	std::vector<gh::command> bot_commands;
	bot_commands.push_back(horn.cmd);
	bot_commands.push_back(registry.add_command);
	bot_commands.push_back(registry.execute_simple_command);

	send_message("HubballBot is now online");

	while (true)
	{
		std::string reply = twitch.receive();
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
			send_message("Received ping from twitch");
		}
		else {
			std::cout << "[UNRECOGNISED]: " << reply << std::endl;
		}
	}

	return 0;
}