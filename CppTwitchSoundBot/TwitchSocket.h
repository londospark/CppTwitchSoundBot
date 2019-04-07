#include <vector>
#include <Windows.h>
#include <Winsock2.h>
#include <string>
#pragma once

namespace gh {
	constexpr int BufferLength = 512;

	class TwitchSocket {
	private:
		std::vector<char> buffer = std::vector<char>(BufferLength);
		SOCKET Connection;
	public:
		int send(const std::string& information);
		std::string receive();
		TwitchSocket(SOCKET Connection);

		TwitchSocket(const TwitchSocket&) = delete;
		TwitchSocket& operator=(const TwitchSocket&) = delete;
	};
}
