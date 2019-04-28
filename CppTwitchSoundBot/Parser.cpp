#include "pch.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include "Parser.h"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace fusion = boost::fusion;
//
//struct twitch_message {};
//
//twitch_message
//parse_message(const std::string& message) {
//	// Get the @ sign
//	// Parse the attrs attributes_for(attrs)
//	// Get the Username
//	// Get the IRC Command (PRIVMSG)
//	// Message Body
//}

user_attributes
attributes_for(const std::string& attr_list) {
	using ascii::char_;
	using qi::parse;
	using fusion::at_c;

	user_attributes results;

	auto kvp = [&results](const fusion::vector<std::vector<char>, std::vector<char>> & pair) {
		auto key = at_c<0>(pair);
		auto value = at_c<1>(pair);
		
		results[std::string(key.begin(), key.end())] = std::string(value.begin(), value.end());
	};

	auto kvp_parser = ((+(char_ - '=')) >> '=' >> (*(char_ - ';')))[kvp];
	auto attr_parser = kvp_parser >> *(';' >> kvp_parser);

	auto begin = attr_list.begin();
	auto end = attr_list.end();

	parse(begin, end, attr_parser);

	return results;
}