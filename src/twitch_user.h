#pragma once

#include <string>
#include <map>

namespace gh
{
	class twitch_user
	{
	public:
		twitch_user static from_attributes(std::string const& attr_list);
		std::string const display_name() const;
		bool const is_moderator() const;
		~twitch_user();

	private:
		twitch_user(std::map<std::string, std::string> attrs);
		std::map<std::string, std::string> attributes;
	};
}

