#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include "TwitchSocket.h"


int main()
{
	std::ifstream file("supersecret.txt");
    std::string OAuthToken;
    
    if (file.is_open())
    {
        std::getline(file, OAuthToken, '\n');
        file.close();
    }
    
	gh::TwitchSocket twitch("garethhubball", OAuthToken);
    
    if (twitch.connect() == 1)
    {
        return 1;
    }
    
	(void)twitch.receive();
    
	twitch.send("JOIN #garethhubball\r\n");
    
	// Parse Message
	// :rhymu8354!rhymu8354@rhymu8354.tmi.twitch.tv PRIVMSG #garethhubball :test
	// :hackur!hackur@hackur.tmi.twitch.tv PRIVMSG #garethhubball :test1
	std::regex re(R"RE(^:([^!]+)![^@]+@[^\.]+\.tmi\.twitch\.tv PRIVMSG #([^\s]+) :(.*))RE");
	std::smatch match;
    
	std::chrono::system_clock clock;
	std::chrono::time_point<std::chrono::system_clock> last_horn;
	last_horn = clock.now() - std::chrono::seconds(30);
    
	while (1)
	{
		std::string reply = twitch.receive();
		std::regex_search(reply, match, re);
		std::cout << reply << std::endl;
		std::cout << "User: " << match[1] << std::endl;
		std::cout << "channel: " << match[2] << " message: " << match[3] << std::endl;
		if (match[3] == "!horn")
		{
			if (clock.now() - last_horn >= std::chrono::seconds(30))
			{
				PlaySound("horn.wav", NULL, SND_FILENAME);
				last_horn = clock.now();
			}
		}
	}
	return 0;
}