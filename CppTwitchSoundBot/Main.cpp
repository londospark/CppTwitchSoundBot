#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include "TwitchSocket.h"

bool BotCommand(const std::string & commandName, const std::string & command, std::string * args) {
	int split = commandName.length() + 1;
	if (command.substr(0, split) == commandName + " ")
	{
		*args = command.substr(split);
		return true;
	}
	return false;
};

int main()
{
	std::ifstream file("supersecret.txt");
    std::string OAuthToken;
    
    if (file.is_open())
    {
        std::getline(file, OAuthToken, '\n');
        file.close();
    }
    
	gh::TwitchSocket twitch("hubballbot", OAuthToken);
    
    if (twitch.connect() == 1)
    {
        return 1;
    }
    
	std::cout << twitch.receive() << std::endl;
    
	twitch.send("JOIN #garethhubball\r\n");
    
	// Parse Message
	// :rhymu8354!rhymu8354@rhymu8354.tmi.twitch.tv PRIVMSG #garethhubball :test
	// :hackur!hackur@hackur.tmi.twitch.tv PRIVMSG #garethhubball :test1
	std::regex re(R"RE(^:([^!]+)![^@]+@[^\.]+\.tmi\.twitch\.tv PRIVMSG #([^\s]+) :(.*))RE");
	std::smatch match;
    
	std::chrono::system_clock clock;
	std::chrono::time_point<std::chrono::system_clock> last_horn;
	last_horn = clock.now() - std::chrono::seconds(30);

	auto sendMessage = twitch.sendToChannel("garethhubball");
    
	sendMessage("HubballBot is now online");

	while (true)
	{
		std::string reply = twitch.receive();
		std::regex_search(reply, match, re);
		std::cout << reply << std::flush;
		std::cout << "User: " << match[1] << std::endl;
		std::cout << "channel: " << match[2] << " message: " << match[3] << std::endl;

		std::string user = match[1].str();
		std::string command = match[3].str();

		std::string arguments;

		if (command == "!horn")
		{
			if (clock.now() - last_horn >= std::chrono::seconds(30))
			{
				PlaySound("horn.wav", NULL, SND_FILENAME);
				last_horn = clock.now();
			}
		}

		if (command == "!random")
		{
			sendMessage("@" + user + " asked for a random number and rolled a 4");
		}

		if (command == "!hi")
		{
			sendMessage("gareth3Hype Hi chat gareth3Hype");
		}

		if (BotCommand("!tnt", command, &arguments))
		{
			sendMessage("Bye for now " + arguments);
		}

		if (BotCommand("!welcome", command, &arguments))
		{
			sendMessage("Welcome to the chat " + arguments + " gareth3Hype");
		}
	}
	return 0;
}