#pragma once
#include <string>
#include <map>
#include <optional>
#include "twitch_user.h"

namespace gh {
	struct twitch_message
	{
		std::optional<twitch_user> user;
		std::string username;
		std::string message_type;
		std::string channel;
		std::string body;

		static std::optional<twitch_message> parse(const std::string& message);
	};
}

