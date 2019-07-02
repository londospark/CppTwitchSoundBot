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
#include <wx/wx.h>
#include <thread>
#include <functional>

#include "twitch_message.h"
#include "command_registry.h"
#include "command_definitions.h"
#include "horn_command.h"
#include "twitch_connection.h"
#include "options.h"
#include "message_handler.h"
#include "main_frame.h"

class HubballBot : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();

	HubballBot();

	asio::io_context io_context;
	std::thread t;
	gh::twitch_connection twitch;
	std::vector<gh::command> bot_commands;
	std::function<size_t(const std::string&)> send_message;
	gh::command_registry registry;
	gh::horn_command horn;
};

wxIMPLEMENT_APP(HubballBot);

HubballBot::HubballBot() : twitch(gh::twitch_connection(io_context)), registry(gh::command_registry("hubballbot.db")), horn(gh::horn_command(30))
{
}

bool HubballBot::OnInit()
{
	gh::MainFrame* frame = new gh::MainFrame();
	frame->Show();

	auto options = gh::options::from_file("config.properties");

	twitch.authenticate(options.username, options.oauth_token);

	frame->log(twitch.receive());

	twitch.send("JOIN #" + options.channel + "\r\n");
	twitch.send("CAP REQ :twitch.tv/tags\r\n");

	send_message = twitch.sendToChannel(options.channel);

	/*if (result != 0)
	{
		std::cout << "Database not opened." << std::endl;
		return result;
	}*/

	registry.load_commands(); //TODO(gareth): Check that we've not had a problem here.
	
	bot_commands.push_back(horn.cmd);
	bot_commands.push_back(registry.add_command);
	bot_commands.push_back(registry.execute_simple_command);

	send_message("HubballBot is online");

	gh::message_handler handler(bot_commands, send_message, twitch);
	twitch.handler = std::move(handler);
	twitch.async_receive();

	t = std::thread([&]() { io_context.run(); });

	return true;
}

int HubballBot::OnExit()
{
	io_context.stop();
	t.join();
	return 0;
}

