#include "pch.hpp"
#include "command_registry.h"
#include "bot_command.h"

namespace gh {

	struct parsed_command {
		std::string name;
		std::optional<std::string> args;
	};

	std::optional<parsed_command> const
		parse(std::string const& raw_command) noexcept
	{
		if (raw_command.empty() || raw_command.front() != '!')
			return std::nullopt;

		auto stripped = raw_command.substr(1);

		if (size_t index = stripped.find_first_of(" ");
			index != std::string::npos)
		{
			return parsed_command {
				stripped.substr(0, index),
				stripped.substr(index + 1)
			};
		}

		return parsed_command { stripped, std::nullopt };
	}

	command_registry::command_registry(std::string const& filename)
		: repo(command_repository(filename))
	{
		add_command = [this](std::string const& body, gh::twitch_user const& user, message_sender send_message) {
			add_command_impl(body, user, send_message);
		};

		execute_simple_command = [this](std::string const& body, gh::twitch_user const& user, message_sender send_message) {
			execute_simple_command_impl(body, user, send_message);
		};
	}

	void command_registry::load_commands()
	{
		simple_commands = repo.load_commands();
	}

	void command_registry::add_command_impl(std::string const& body, gh::twitch_user const& user, message_sender send_message)
	{
		if (auto arguments = arguments_for_command("!addcom", body); user.is_moderator())
		{
			// !addcom youtube Follow Gareth on youtube...

			if (int index = arguments->find_first_of(" "); index != std::string::npos) {
				std::string command_to_add;
				std::string reply;

				command_to_add = arguments->substr(0, index);
				reply = arguments->substr(static_cast<size_t>(index) + 1);

				repo.add_command(command_to_add, reply);
				simple_commands[command_to_add] = reply;
			}
		}
	}

	std::string const
		transform(std::string const& command_template, std::optional<std::string> const& args, gh::twitch_user const& user)
	{
		std::string toReturn = command_template;

		std::map<std::string, std::string> replacements {
			std::make_pair("user", user.display_name())
		};

		if (auto touser = args)
		{
			replacements["touser"] = *touser;
		}

		for (auto replacement : replacements)
		{
			auto token = "{" + replacement.first + "}";
			if (size_t pos = command_template.find(token); pos != std::string::npos)
			{
				toReturn =
					command_template.substr(0, pos)
					+ replacement.second
					+ command_template.substr(pos + token.length());
			}
		}
		return toReturn;
	}

	void command_registry::execute_simple_command_impl(std::string const& body, gh::twitch_user const& user, message_sender send_message)
	{
		if (auto parsed = parse(body)) {
			auto command = simple_commands.find(parsed->name);
			if (command != simple_commands.end())
			{
				auto reply = transform(command->second, parsed->args, user);
				send_message(reply);
			}
			else
			{
				std::cout << "Command: " << body.substr(1) << " not found" << std::endl;
			}
		}
	}
}