#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <any>
#include <functional>
#include "MsgData.hpp"
#include "DataManager.hpp"

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

using DataReceiveCallback = std::function<void(MsgDataPtr)>;

class Endpoint
{
public:
    Endpoint(SOCKET clientSocket, unsigned int bufferSize, DataReceiveCallback callback); // Also pass in data manager as event. When data is received, add data to data manager.

	~Endpoint() = default;

	void setBufferSize(size_t bufferSize)
	{
		bufferSize_ = bufferSize;
	}

	const size_t getBufferSize()
	{
		return bufferSize_;
	}

	bool sendData(const std::any& data, size_t dataSize);

	bool valid();

	void cleanup();

private:
	bool valid_;
	DataReceiveCallback callback_;
	int iResult_;
    SOCKET clientSocket_;
	size_t bufferSize_;
	char recvbuf_[DEFAULT_BUFLEN];
	int recvbuflen_;
};