#include "pch.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include "twitch_message.h"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace fusion = boost::fusion;

std::string to_string(std::vector<char> const & vec)
{
	return std::string(vec.begin(), vec.end());
}

namespace gh {

	std::optional<twitch_message>
		twitch_message::parse(std::string const& message) {
		using ascii::char_;

		twitch_message msg;

		const auto attrs = [&msg](auto attributes) -> void {
			msg.user = twitch_user::from_attributes(to_string(attributes));
		};

		const auto user = [&msg](auto username) -> void {
			msg.username = to_string(username);
		};

		const auto msgType = [&msg](auto type) -> void {
			msg.message_type = to_string(type);
		};

		const auto channel = [&msg](auto channel) -> void {
			msg.channel = to_string(channel);
		};

		const auto body = [&msg](auto body) -> void {
			msg.body = to_string(body);
		};

		// @key=value;key=value :rhymu8354!rhymu8354@rhymu8354.tmi.twitch.tv PRIVMSG #garethhubball :test
		//std::regex re(R"RE(^@(.+) :([^!]+)![^@]+@[^\.]+\.tmi\.twitch\.tv ([^\s]+) #([^\s]+) :(.*))RE");
		auto messageParser =
			boost::proto::deep_copy(
				'@'
				>> (+(char_ - ' '))[attrs] >> " :"
				>> (+(char_ - '!'))[user]
				>> (+(char_ - '@')) >> '@'
				>> (+(char_ - '.')) >> '.'
				>> "tmi.twitch.tv "
				>> (+(char_ - ' '))[msgType] >> " #"
				>> (+(char_ - ' '))[channel] >> " :"
				>> (+(char_ - '\r'))[body] >> "\r\n");

		if (qi::parse(message.begin(), message.end(), messageParser))
		{
			return msg;
		}
		return {};
	}
}
