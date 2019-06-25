#pragma once

#include <vector>
#include <functional>
#include <string>

#include "twitch_connection.h"
#include "command_definitions.h"

namespace gh {
	class message_handler {

	public:

		message_handler(std::vector<gh::command> const& bot_commands,
			std::function<size_t(const std::string&)> const& send_message,
			gh::twitch_connection& twitch);

		void operator()(std::string const& reply);

	private:
		std::vector<gh::command> const& bot_commands;
		std::function<size_t(const std::string&)> const& send_message;
		gh::twitch_connection& twitch;
	};
}