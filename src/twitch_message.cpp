#include "twitch_message.h"

#include <regex>

namespace gh {

	std::optional<twitch_message>
		twitch_message::parse(std::string const& message) {

		twitch_message msg;

		// @key=value;key=value :rhymu8354!rhymu8354@rhymu8354.tmi.twitch.tv PRIVMSG #garethhubball :test
		std::regex re(R"RE(^@(.+) :([^!]+)![^@]+@[^\.]+\.tmi\.twitch\.tv ([^\s]+) #([^\s]+) :(.*))RE");
		std::smatch match;

		if (std::regex_search(message, match, re)) {
			msg.user = twitch_user::from_attributes(match[1]);
			msg.message_type = match[3];
			msg.channel = match[4];
			msg.body = match[5];

			return msg;
		}
		else {
			return {};
		}
	}
}
