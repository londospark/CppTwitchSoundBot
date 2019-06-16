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
	boost::program_options::options_description desc;
	desc.add_options()
		("authentication.username", "username of the bot")
		("authentication.oauth_token", "Your bot's OAuth Token (starts with oauth:)")
		("settings.channel", "Channel to join (your username)");
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_config_file("config.properties", desc), vm);
	boost::program_options::notify(vm);

	auto username = vm.at("authentication.username").as<std::string>();
	auto oauth_token = vm.at("authentication.oauth_token").as<std::string>();
	auto channel = vm.at("settings.channel").as<std::string>();

	boost::asio::io_context io_context;
	gh::twitch_connection twitch(io_context);
	twitch.authenticate(username, oauth_token);

	std::cout << twitch.receive() << std::endl;

	twitch.send("JOIN #" + channel + "\r\n");
	twitch.send("CAP REQ :twitch.tv/tags\r\n");

	auto const send_message = twitch.sendToChannel(channel);
	
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