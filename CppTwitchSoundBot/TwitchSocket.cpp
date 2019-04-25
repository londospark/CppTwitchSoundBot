#include <Winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <iostream>
#include "TwitchSocket.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

namespace gh
{
    int twitch_socket::send(const std::string &information)
    {
        return ::send(this->Connection, information.c_str(), information.length(), NULL);
    }

	std::function<int(const std::string&)> twitch_socket::sendToChannel(const std::string& channel)
	{
		return [=](const std::string & message) -> int {
			return this->send("PRIVMSG #" + channel + " :" + message + "\r\n");
		};
	}
    
    std::string twitch_socket::receive()
    {
        int bytesReceived = recv(this->Connection, buffer.data(), BufferLength, NULL);
        if (bytesReceived < 0)
        {
            return "";
        }
        
        return std::string(buffer.begin(), buffer.begin() + bytesReceived);
    }
    
    twitch_socket::twitch_socket(std::string username, std::string password)
    {
		this->Connection = INVALID_SOCKET;
        this->username = username;
        this->password = password;
    }
    
    int twitch_socket::connect()
    {
        WSADATA wsaData;
        addrinfo hints{};
        addrinfo *result;
        
        
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cout << "Startup Failed" << std::endl;
            return 1;
        }
        
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        
        if (getaddrinfo("irc.chat.twitch.tv", "6667", &hints, &result) != 0)
        {
            std::cout << "getaddrinfo failed: <<RETURN CODE CAN BE PASSED>>" << std::endl;
        }
        
        this->Connection = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (this->Connection == INVALID_SOCKET)
        {
            std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }
        
        if (::connect(this->Connection, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR)
        {
            std::cout << "[ERR] Connected with an error" << std::endl;
            closesocket(this->Connection);
            this->Connection = INVALID_SOCKET;
            return 1;
        }
        
        freeaddrinfo(result);
        
        if (this->Connection == INVALID_SOCKET)
        {
            std::cout << "Cannot connect to server" << std::endl;
            WSACleanup();
            return 1;
        }
        
        send("PASS " + this->password + "\r\n");
        send("NICK " + this->username + "\r\n");
        return 0;
    }
}