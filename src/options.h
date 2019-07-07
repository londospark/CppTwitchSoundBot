#pragma once

#include <string>
#include <optional>

namespace gh {

	class options {
	public:
		std::string username;
		std::string oauth_token;
		std::string channel;

		options(std::string const& username, std::string const& oauth_token, std::string const& channel);
		static std::optional<options> from_file(std::string const& filename);
	};

}