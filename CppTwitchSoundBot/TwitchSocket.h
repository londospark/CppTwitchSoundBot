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
    
	class TwitchSocket {
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
		int send(const std::string& information);
		std::string receive();
        int connect();
		TwitchSocket(std::string username, std::string password);
		std::function<int(const std::string&)> sendToChannel(const std::string& channel);
        
		TwitchSocket(const TwitchSocket&) = delete;
		TwitchSocket& operator=(const TwitchSocket&) = delete;
	};
}
