#include "options.h"

#include <fstream>
#include <sstream>
#include <map>

namespace gh {

	options::options(std::string const& username, std::string const& oauth_token, std::string const& channel) : username(username), oauth_token(oauth_token), channel(channel) {};

	std::optional<std::map<std::string, std::string>> parse_file(std::ifstream& config_file)
	{
		auto settings = std::map<std::string, std::string>();
		std::string current_category{};
		std::string line{};
		while (std::getline(config_file, line))
		{
			if (line.length() == 0) {
				continue;
			}
			else if (line.at(0) == '[') {
				current_category = std::string(line.begin() + 1, line.end() - 1);
			}
			else if (auto split_at = line.find('='); split_at != std::string::npos) {
				auto key = current_category + "." + std::string(line.begin(), line.begin() + split_at);
				auto value = std::string(line.begin() + split_at + 1, line.end());
				settings[key] = value;
			}
			else {
				return {};
			}
		}

		return settings;
	}

	std::optional<options> options::from_file(std::string const& filename)
	{
		auto config_file = std::ifstream(filename);
		if (auto parsed = parse_file(config_file)) {
			auto username = parsed->find("authentication.username")->second;
			auto oauth_token = parsed->find("authentication.oauth_token")->second;
			auto channel = parsed->find("settings.channel")->second;
			return options(username, oauth_token, channel);
		}
		else {
			return {};
		}
	}
}
