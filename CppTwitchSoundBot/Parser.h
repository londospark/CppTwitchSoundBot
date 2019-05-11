#pragma once
#include <string>
#include <map>

using user_attributes = std::map<std::string, std::string>;

struct twitch_message
{
	user_attributes attributes;
	std::string username;
	std::string message_type;
	std::string channel;
	std::string body;
};

user_attributes attributes_for(const std::string& attrList);
bool parse_message(const std::string& message, twitch_message& msg);