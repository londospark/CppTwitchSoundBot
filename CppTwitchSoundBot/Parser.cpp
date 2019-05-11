#include "pch.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include "Parser.h"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace fusion = boost::fusion;

std::string to_string(const std::vector<char> & vec)
{
	return std::string(vec.begin(), vec.end());
}

bool
parse_message(const std::string & message, twitch_message & msg) {
	using ascii::char_;
	
	const auto attrs = [&msg](auto attributes) -> void {
		msg.attributes = attributes_for(to_string(attributes));
	};

	const auto user = [&msg](auto username) -> void {
		msg.username = to_string(username);
	};

	const auto msgType = [&msg](auto type) -> void {
		msg.message_type = to_string(type);
	};

	const auto channel = [&msg](auto channel) -> void {
		msg.channel = to_string(channel);
	};

	const auto body = [&msg](auto body) -> void {
		msg.body = to_string(body);
	};

	// @key=value;key=value :rhymu8354!rhymu8354@rhymu8354.tmi.twitch.tv PRIVMSG #garethhubball :test
	//std::regex re(R"RE(^@(.+) :([^!]+)![^@]+@[^\.]+\.tmi\.twitch\.tv ([^\s]+) #([^\s]+) :(.*))RE");
	auto messageParser =
		'@'
		>> (+(char_ - ' '))[attrs] >> " :"
		>> (+(char_ - '!'))[user]
		>> (+(char_ - '@')) >> '@'
		>> (+(char_ - '.')) >> '.'
		>> "tmi.twitch.tv "
		>> (+(char_ - ' '))[msgType] >> " #"
		>> (+(char_ - ' '))[channel] >> " :"
		>> (+(char_ - '\r'))[body] >> "\r\n";


	return qi::parse(message.begin(), message.end(), messageParser);
}


user_attributes
attributes_for(const std::string & attr_list) {
	using ascii::char_;
	using qi::parse;
	using fusion::at_c;

	user_attributes results;

	const auto kvp = [&results](const fusion::vector<std::vector<char>, std::vector<char>> & pair) {
		auto key = at_c<0>(pair);
		auto value = at_c<1>(pair);
		
		results[std::string(key.begin(), key.end())] = to_string(value);
	};

	const auto kvp_parser = ((+(char_ - '=')) >> '=' >> (*(char_ - ';')))[kvp];
	const auto attr_parser = kvp_parser >> *(';' >> kvp_parser);

	parse(attr_list.begin(), attr_list.end(), attr_parser);

	return results;
}