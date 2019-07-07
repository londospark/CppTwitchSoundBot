#include "twitch_user.h"

namespace gh {
	twitch_user::twitch_user(std::map<std::string, std::string> attrs)
		: attributes(std::move(attrs)) { }

	twitch_user
		twitch_user::from_attributes(std::string const& attr_list)
	{
		std::map<std::string, std::string> attributes{};
		size_t current_position{};
		size_t next_position{};

		while (current_position != std::string::npos)
		{
			auto split = attr_list.find('=', next_position);
			auto key = attr_list.substr(next_position, split - next_position);

			current_position = attr_list.find(';', split);
			next_position = current_position + 1;

			auto value = attr_list.substr(split + 1, current_position - split - 1);
			attributes[key] = value;
		}

		return twitch_user(std::move(attributes));
	}

	std::string const
		twitch_user::display_name() const
	{
		if (auto it = attributes.find("display-name"); it != attributes.end())
		{
			return it->second;
		}
		return "";
	}

	//TODO(gareth): Cleanup to include the broadcaster
	bool const
		twitch_user::is_moderator() const
	{
		if (display_name() == "GarethHubball")
			return true;

		if (auto it = attributes.find("user-type"); it != attributes.end())
		{
			return it->second == "mod";
		}
		return false;
	}

	twitch_user::~twitch_user() { }
}