#pragma once
#include <optional>
#include <string>

namespace {
	std::optional<std::string> const
		arguments_for_command(std::string const& commandName, std::string const& command) {
		size_t split = commandName.length() + 1;
		if (command.substr(0, split) == commandName + " ")
		{
			return command.substr(split);
		}

		return std::nullopt;
	};
}