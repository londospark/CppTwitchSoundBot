#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <map>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "TwitchSocket.h"

using user_attributes = std::map<std::string, std::string>;

bool
bot_command(const std::string & commandName, const std::string & command, std::string * args) {
	int split = commandName.length() + 1;
	if (command.substr(0, split) == commandName + " ")
	{
		*args = command.substr(split);
		return true;
	}

	return false;
};

user_attributes
attributes_for(const std::string & attrList) {
	user_attributes results;
	
	std::vector<std::string> keyValuePairs;
	boost::algorithm::split(keyValuePairs, attrList, boost::algorithm::is_any_of(";"));
	
	for (const auto & kvp : keyValuePairs)
	{
		std::vector<std::string> elements;
		boost::algorithm::split(elements, kvp, boost::algorithm::is_any_of("="));
		results[elements.front()] = elements.back();
	}

	return results;
}

bool
is_moderator(const user_attributes& attributes)
{
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
	// :rhymu8354!rhymu8354@rhymu8354.tmi.twitch.tv PRIVMSG #garethhubball :test
	// :hackur!hackur@hackur.tmi.twitch.tv PRIVMSG #garethhubball :test1
	std::regex re(R"RE(^@(.+) :([^!]+)![^@]+@[^\.]+\.tmi\.twitch\.tv PRIVMSG #([^\s]+) :(.*))RE");
	std::smatch match;

	std::chrono::system_clock clock;
	std::chrono::time_point<std::chrono::system_clock> last_horn;
	last_horn = clock.now() - std::chrono::seconds(30);

	auto send_message = twitch.sendToChannel("garethhubball");
    
	send_message("HubballBot is now online");

	while (true)
	{
		std::string reply = twitch.receive();
		std::regex_search(reply, match, re);

		std::map<std::string, std::string> attributes = attributes_for(match[1]);
		
		std::cout << reply << std::flush;
		std::cout << "Attributes: " << match[1] << std::endl;
		std::cout << "User: " << match[2] << std::endl;
		std::cout << "channel: " << match[3] << " message: " << match[4] << std::endl;

		std::string user = match[2].str();
		std::string command = match[4].str();

		std::string arguments;

		if (command == "!horn" && is_moderator(attributes))
		{
			auto lastUsed = clock.now() - last_horn;
			if (lastUsed >= std::chrono::seconds(30))
			{
				PlaySound("horn.wav", NULL, SND_FILENAME);
				last_horn = clock.now();
			}
			else
			{
				auto waitingTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::seconds(30) - lastUsed).count();
				std::stringstream stream;
				stream << "Horn in cooldown, please wait " << waitingTime << " seconds";
				send_message(stream.str());
			}
		}

		if (command == "!random")
		{
			send_message("@" + user + " asked for a random number and rolled a 4");
		}

		if (command == "!hi")
		{
			send_message("gareth3Hype Hi chat gareth3Hype");
		}

		if (bot_command("!tnt", command, &arguments))
		{
			send_message("Bye for now " + arguments);
		}

		if (command == "!raidcall")
		{
			send_message("Subscribers - please use the following raid call:");
			send_message("gareth3Hype gareth3Hype gareth3Hype YAPPY DOG RAID gareth3Hype gareth3Hype gareth3Hype");
			send_message("Non-subscribers: ");
			send_message("YAPPY DOG RAID! YAPPY DOG RAID!");
		}

		if (bot_command("!welcome", command, &arguments))
		{
			send_message("Welcome to the chat " + arguments + " gareth3Hype");
		}
	}
	return 0;
}