#include "twitch_user.h"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace fusion = boost::fusion;

namespace gh {
	twitch_user::twitch_user(std::map<std::string, std::string> attrs)
		: attributes(std::move(attrs)) { }

	twitch_user
		twitch_user::from_attributes(std::string const& attr_list)
	{
		using ascii::char_;
		using qi::parse;
		using fusion::at_c;
		using boost::proto::deep_copy;

		std::map<std::string, std::string> attributes;

		const auto kvp = [&attributes](fusion::vector<std::vector<char>, std::vector<char>> const& pair) {
			auto key = at_c<0>(pair);
			auto value = at_c<1>(pair);

			attributes[std::string(key.begin(), key.end())] = std::string(value.begin(), value.end());
		};

		const auto attr_parser = deep_copy(
			(((+(char_ - '=')) >> '=' >> (*(char_ - ';')))[kvp]) % ';'
		);

		parse(attr_list.begin(), attr_list.end(), attr_parser);

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