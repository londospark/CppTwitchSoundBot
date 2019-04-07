#include <Winsock2.h>
#include <Windows.h>
#include <string>
#include <vector>
#include "TwitchSocket.h"

namespace gh {
	int TwitchSocket::send(const std::string& information) {
		return ::send(Connection, information.c_str(), information.length(), NULL);
	}

	std::string TwitchSocket::receive() {
		int bytesReceived = recv(Connection, buffer.data(), BufferLength, NULL);
		return std::string(buffer.begin(), buffer.begin() + bytesReceived);
	}

	TwitchSocket::TwitchSocket(SOCKET Connection) {
		this->Connection = Connection;
	}
}