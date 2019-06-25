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
#include "command_registry.h"
#include "command_definitions.h"
#include "horn_command.h"
#include "twitch_connection.h"
#include "options.h"
#include "message_handler.h"

int main()
{
	auto options = gh::options::from_file("config.properties");

	boost::asio::io_context io_context;
	gh::twitch_connection twitch(io_context);
	twitch.authenticate(options.username, options.oauth_token);

	std::cout << twitch.receive() << std::endl;

	twitch.send("JOIN #" + options.channel + "\r\n");
	twitch.send("CAP REQ :twitch.tv/tags\r\n");

	auto const send_message = twitch.sendToChannel(options.channel);
	
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

	gh::message_handler handler(bot_commands, send_message, twitch);
	twitch.async_receive(handler);

	io_context.run();

	return 0;
}