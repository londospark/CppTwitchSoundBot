#pragma once

#include "twitch_user.h"
#include "command_repository.h"
#include "command_definitions.h"

namespace gh {
	class command_registry
	{
	public:
		command_registry(std::string const& filename);
		void load_commands();
		command add_command;
		command execute_simple_command;

	private:
		std::map<std::string, std::string> simple_commands;
		command_repository repo;
		
		void add_command_impl(std::string const& body, gh::twitch_user const& user, message_sender send_message);

		void execute_simple_command_impl(std::string const& body, gh::twitch_user const& user, message_sender send_message);
	};
}