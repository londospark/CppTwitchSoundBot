#include "pch.hpp"
#include "horn_command.h"
namespace gh {

	horn_command::horn_command(int cooldown) : cooldown(cooldown)
	{
		using namespace std::placeholders;
		cmd = std::bind(&horn_command::cmd_impl, this, _1, _2, _3);
		last_horn = clock.now() - std::chrono::seconds(cooldown);
	}

	void horn_command::cmd_impl(std::string const& body, gh::twitch_user const& user, message_sender send_message) {
		//TODO(gareth): Look at throttling in commands before pulling this one out.
		if (body == "!horn" && user.is_moderator())
		{
			auto lastUsed = clock.now() - last_horn;
			if (lastUsed >= std::chrono::seconds(cooldown))
			{
				PlaySound("horn.wav", NULL, SND_FILENAME);
				last_horn = clock.now();
			}
			else
			{
				using std::chrono::seconds;
				using std::chrono::duration_cast;

				auto waitingTime = duration_cast<seconds>(seconds(cooldown) - lastUsed).count();
				std::stringstream stream;
				stream << "Horn in cooldown, please wait " << waitingTime << " seconds";
				send_message(stream.str());
			}
		}
	}
}