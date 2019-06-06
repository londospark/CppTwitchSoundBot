#pragma once
#include <functional>
#include "twitch_user.h"
namespace gh {
	using message_sender = std::function<int(std::string const&)>;
	using command = std::function<void(std::string const&, gh::twitch_user const&, message_sender)>;
}