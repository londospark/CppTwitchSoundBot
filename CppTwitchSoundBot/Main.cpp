#include <Winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include "TwitchSocket.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

int main() {
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	addrinfo hints{ };
	addrinfo* result;
	std::ifstream file("supersecret.txt");
	std::string OAuthToken;

	if (file.is_open()) {
		std::getline(file, OAuthToken, '\n');
		file.close();
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "Startup Failed" << std::endl;
		return 1;
	}
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo("irc.chat.twitch.tv", "6667", &hints, &result) != 0) {
		std::cout << "getaddrinfo failed: <<RETURN CODE CAN BE PASSED>>" << std::endl;
	}

	ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	if (connect(ConnectSocket, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Cannot connect to server" << std::endl;
		WSACleanup();
		return 1;
	}

	gh::TwitchSocket twitch(ConnectSocket);
	twitch.send("PASS " + OAuthToken + "\r\n");
	twitch.send("NICK garethhubball\r\n");
	
	(void)twitch.receive();

	twitch.send("JOIN #garethhubball\r\n");

	// Parse Message
	// :rhymu8354!rhymu8354@rhymu8354.tmi.twitch.tv PRIVMSG #garethhubball :test
	// :hackur!hackur@hackur.tmi.twitch.tv PRIVMSG #garethhubball :test1
	std::regex re(R"RE(^:([^!]+)![^@]+@[^\.]+\.tmi\.twitch\.tv PRIVMSG #([^\s]+) :(.*))RE");
	std::smatch match;
	
	std::chrono::system_clock clock;
	std::chrono::time_point<std::chrono::system_clock> last_horn;
	last_horn = clock.now() - std::chrono::seconds(30);

	while (1) {
		std::string reply = twitch.receive();
		std::regex_search(reply, match, re);
		std::cout << reply << std::endl;
		std::cout << "User: " << match[1] << std::endl;
		std::cout << "channel: " << match[2] << " message: " << match[3] << std::endl;
		if (match[3] == "!horn") {
			if (clock.now() - last_horn >= std::chrono::seconds(30)) {
				PlaySound("horn.wav", NULL, SND_FILENAME);
				last_horn = clock.now();
			}
		}
	}
	return 0;
}