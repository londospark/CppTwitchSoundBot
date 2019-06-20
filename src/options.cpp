#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "options.h"

namespace gh {

	options::options(std::string const& username, std::string const& oauth_token, std::string const& channel) : username(username), oauth_token(oauth_token), channel(channel) {};

	options options::from_file(std::string const& filename)
	{
		using namespace boost::program_options;

		options_description desc;
		desc.add_options()
			("authentication.username", "username of the bot")
			("authentication.oauth_token", "Your bot's OAuth Token (starts with oauth:)")
			("settings.channel", "Channel to join (your username)");

		variables_map vm;
		store(boost::program_options::parse_config_file(filename.c_str(), desc), vm);
		notify(vm);

		auto username = vm.at("authentication.username").as<std::string>();
		auto oauth_token = vm.at("authentication.oauth_token").as<std::string>();
		auto channel = vm.at("settings.channel").as<std::string>();
		return options(username, oauth_token, channel);
	}
}