// TODO(gareth): Add in the BSD socket library here for *NIX platforms
#ifdef TB_WINDOWS
#elif defined TB_UNIX
#else
#error "Please define wither TB_WINDOWS or TB_UNIX"
#endif

#include <vector>
#include <string>
#include <stdint.h>
#include <functional>
#pragma once

namespace gh {
	constexpr int BufferLength = 512;
    
	class twitch_socket {
    private:
		std::vector<char> buffer = std::vector<char>(BufferLength);
		std::string username;
        std::string password;
#ifdef TB_WINDOWS
        //NOTE(gareth): I wonder if the BSD socket types are pretty much u64 or castable to them, let's find out - that would be fun!
		uint64_t Connection;
#elif defined TB_UNIX 
#else
#error "Please define wither TB_WINDOWS or TB_UNIX"
#endif
        
    public:
		twitch_socket(std::string username, std::string password);

		int connect();

		int send(const std::string& information);
		std::function<int(const std::string&)> sendToChannel(const std::string& channel);

		std::string receive();
		        
		twitch_socket(const twitch_socket&) = delete;
		twitch_socket& operator=(const twitch_socket&) = delete;
	};
}
