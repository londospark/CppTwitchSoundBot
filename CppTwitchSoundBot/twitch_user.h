#pragma once

#include <string>
#include <map>

namespace gh {
	class twitch_user
	{
	public:
		twitch_user static from_attributes(std::string const& attr_list);
		bool const is_moderator();
		~twitch_user();

	private:
		twitch_user(std::map<std::string, std::string> attrs);
		std::map<std::string, std::string> attributes;
	};
}

