#include "pch.hpp"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <map>
#include <sstream>
#include <codecvt>
#include "command_repository.h"
#include "TwitchSocket.h"
#include "Parser.h"

bool
bot_command(std::string const& commandName, std::string const& command, std::string* args) {
	size_t split = commandName.length() + 1;
	if (command.substr(0, split) == commandName + " ")
	{
		*args = command.substr(split);
		return true;
	}

	return false;
};

//TODO: Cleanup to include the broadcaster
bool const
is_moderator(user_attributes& const attributes)
{
	if (auto it = attributes.find("display-name"); it != attributes.end())
	{
		if (it->second == "GarethHubball")
			return true;
	}

	if (auto it = attributes.find("user-type"); it != attributes.end())
	{
		return it->second == "mod";
	}
	return false;
}

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

	gh::twitch_socket twitch("hubballbot", oauth_token);

	if (twitch.connect() == 1)
	{
		return 1;
	}

	std::cout << twitch.receive() << std::endl;

	twitch.send("JOIN #garethhubball\r\n");
	twitch.send("CAP REQ :twitch.tv/tags\r\n");

	// Parse Message
	// @key=value;key=value :rhymu8354!rhymu8354@rhymu8354.tmi.twitch.tv PRIVMSG #garethhubball :test
	// :hackur!hackur@hackur.tmi.twitch.tv PRIVMSG #garethhubball :test1
	//std::regex re(R"RE(^@(.+) :([^!]+)![^@]+@[^\.]+\.tmi\.twitch\.tv PRIVMSG #([^\s]+) :(.*))RE");
	//std::smatch match;

	std::chrono::system_clock clock;
	std::chrono::time_point<std::chrono::system_clock> last_horn;
	last_horn = clock.now() - std::chrono::seconds(30);

	auto const send_message = twitch.sendToChannel("garethhubball");

	gh::command_repository repo("hubballbot.db");

	/*if (result != 0)
	{
		std::cout << "Database not opened." << std::endl;
		return result;
	}*/

	std::map<std::string, std::string> commands = repo.load_commands();

	send_message("HubballBot is now online");

	while (true)
	{
		std::string reply = twitch.receive();
		twitch_message message;
		if (parse_message(reply, message))
		{
			std::map<std::string, std::string> attributes = message.attributes;

			std::cout << reply << std::flush;
			std::cout << "User: " << message.username << std::endl;
			std::cout << "channel: " << message.channel << " message: " << message.body << std::endl;
			
			std::string arguments;

			if (message.body == "!horn" && is_moderator(attributes))
			{
				auto lastUsed = clock.now() - last_horn;
				if (lastUsed >= std::chrono::seconds(30))
				{
					PlaySound("horn.wav", NULL, SND_FILENAME);
					last_horn = clock.now();
				}
				else
				{
					using std::chrono::seconds;
					using std::chrono::duration_cast;

					auto waitingTime = duration_cast<seconds>(seconds(30) - lastUsed).count();
					std::stringstream stream;
					stream << "Horn in cooldown, please wait " << waitingTime << " seconds";
					send_message(stream.str());
				}
			}

			if (is_moderator(attributes) && bot_command("!addcom", message.body, &arguments))
			{
				// !addcom youtube Follow Gareth on youtube...
			
				int index = arguments.find_first_of(" ");
				if (index != std::string::npos) {
					std::string command_to_add;
					std::string reply;
					
					command_to_add = arguments.substr(0, index);
					reply = arguments.substr(index + 1);

					repo.add_command(command_to_add, reply);
					commands[command_to_add] = reply;
				}
			}

			if (message.body == "!random")
			{
				send_message("@" + message.username + " asked for a random number and rolled a 4");
			}

			if (bot_command("!tnt", message.body, &arguments))
			{
				send_message("gareth3Hype gareth3Hype Until next time " + arguments + " gareth3Hype gareth3Hype");
			}

			if (bot_command("!welcome", message.body, &arguments))
			{
				send_message("Welcome to the chat " + arguments + " gareth3Hype");
			}

			if (!message.body.empty() && message.body.front() == '!')
			{
				auto command = commands.find(message.body.substr(1));
				if (command != commands.end())
				{
					send_message(command->second);
				}
				else
				{
					std::cout << "Command: " << message.body.substr(1) << " not found" << std::endl;
				}
			}
		}
	}
		
	return 0;
}