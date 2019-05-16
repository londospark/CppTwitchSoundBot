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
#include <optional>
#include "command_repository.h"
#include "twitch_socket.h"
#include "twitch_message.h"

using message_sender = std::function<int(std::string const&)>;
using command = std::function<void(std::string const&, gh::twitch_user const&, message_sender)>;

namespace {

	std::optional<std::string>
		bot_command(std::string const& commandName, std::string const& command) {
		size_t split = commandName.length() + 1;
		if (command.substr(0, split) == commandName + " ")
		{
			return command.substr(split);
		}

		return {};
	};

	void
		command_random(std::string const& body, gh::twitch_user const& user, message_sender send_message) {
		if (body == "!random")
		{
			send_message("@" + user.display_name() + " asked for a random number and rolled a 4");
		}
	}

	void
		command_tnt(std::string const& body, gh::twitch_user const& user, message_sender send_message)
	{
		if (auto arguments = bot_command("!tnt", body))
		{
			send_message("gareth3Hype gareth3Hype Until next time " + *arguments + " gareth3Hype gareth3Hype");
		}
	}

	void
		command_welcome(std::string const& body, gh::twitch_user const& user, message_sender send_message)
	{
		if (auto arguments = bot_command("!welcome", body))
		{
			send_message("Welcome to the chat " + *arguments + " gareth3Hype");
		}
	}

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
		if (auto message = gh::twitch_message::parse(reply))
		{
			std::cout << reply << std::flush;
			std::cout << "User: " << message->username << std::endl;
			std::cout << "channel: " << message->channel << " message: " << message->body << std::endl;
			

			if (auto user = message->user)
			{
				std::string arguments;
				std::vector<command> bot_commands;
				bot_commands.push_back(command_random);
				bot_commands.push_back(command_tnt);
				bot_commands.push_back(command_welcome);

				for (auto command_impl : bot_commands)
				{
					command_impl(message->body, *user, send_message);
				}

				 //TODO(gareth): Look at throttling in commands before pulling this one out.
				if (message->body == "!horn" && user->is_moderator())
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

				//TODO(gareth): Command registry with management functions exported as commands
				if (auto arguments = bot_command("!addcom", message->body); user->is_moderator())
				{
					// !addcom youtube Follow Gareth on youtube...

					int index = arguments->find_first_of(" ");
					if (index != std::string::npos) {
						std::string command_to_add;
						std::string reply;

						command_to_add = arguments->substr(0, index);
						reply = arguments->substr(index + 1);

						repo.add_command(command_to_add, reply);
						commands[command_to_add] = reply;
					}
				}

				if (!message->body.empty() && message->body.front() == '!')
				{
					auto command = commands.find(message->body.substr(1));
					if (command != commands.end())
					{
						send_message(command->second);
					}
					else
					{
						std::cout << "Command: " << message->body.substr(1) << " not found" << std::endl;
					}
				}
			}
		}
	}
		
	return 0;
}