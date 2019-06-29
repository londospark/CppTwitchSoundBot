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
#include <boost/thread.hpp>

#include "twitch_message.h"
#include "command_registry.h"
#include "command_definitions.h"
#include "horn_command.h"
#include "twitch_connection.h"
#include "options.h"
#include "message_handler.h"


class HubballBot : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();

	HubballBot();

	boost::asio::io_context io_context;
	boost::thread t;
	gh::twitch_connection twitch;
	std::vector<gh::command> bot_commands;
	std::function<size_t(const std::string&)> send_message;
	gh::command_registry registry;
	gh::horn_command horn;
};

class MainFrame : public wxFrame
{
public:
	MainFrame();
	void log(std::string const& message);

private:
	wxTextCtrl* logtext;
};

wxIMPLEMENT_APP(HubballBot);

HubballBot::HubballBot() : twitch(gh::twitch_connection(io_context)), registry(gh::command_registry("hubballbot.db")), horn(gh::horn_command(30))
{
}

bool HubballBot::OnInit()
{
	MainFrame* frame = new MainFrame();
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

	t = boost::thread(boost::bind(&boost::asio::io_context::run, &io_context));

	return true;
}

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "HubballBot")
{
	wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
	logtext = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(100, 60), wxTE_MULTILINE);
	// create text ctrl with minimal size 100x60
	topsizer->Add(
		logtext,
		1,            // make vertically stretchable
		wxEXPAND |    // make horizontally stretchable
		wxALL,        //   and make border all around
		10);         // set border width to 10
}

void MainFrame::log(std::string const& message) {
	std::ostream stream(logtext);
    stream << message << '\n';
}

int HubballBot::OnExit()
{
	io_context.stop();
	t.join();
	return 0;
}