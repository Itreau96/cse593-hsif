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
#include <queue>

/*
Copyright 2020 Itreau Bigsby

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

@author   Itreau Bigsby    mailto:ibigsby@asu.edu
 */

#pragma comment (lib, "Ws2_32.lib")

#define PACKET_SIZE 1024

/// <summary>
/// Used to encapsulate client socket information. Provides functionality for 
/// sending, and receiving data using windows sockets.
/// </summary>
class Endpoint
{
public:
	/// <summary>
	/// Default constructor/destructors
	/// </summary>
	Endpoint() = delete;
	~Endpoint() = default;

    /// <summary>
    /// Primary constructor. Accepts an existing socket connection as input.
    /// </summary>
    /// <param name="clientSocket">Existing client connection.</param>
    Endpoint(SOCKET clientSocket);

	/// <summary>
	/// Receives JSON message string to process as socket input.
	/// </summary>
	/// <param name="msg">JSON string message.</param>
	void receiveMsg(std::string msg);

	/// <summary>
	/// Processes received packet.
	/// </summary>
	/// <returns>Returns whether or not process was successful.</returns>
	bool processRecv();

	/// <summary>
	/// Signals to the endpoint that the previous packet has successfully sent
	/// and its resources can be cleared.
	/// </summary>
	void bufferReady();

	/// <summary>
	/// Closes socket connection.
	/// </summary>
	void cleanup();

	// Operates as contiguous message inbox string
	std::string inbox;
	
	// Operates as outgoing message queue
	std::queue<std::string> outbox;

	// Winsock intermediary buffer resources
	char sendBuffer[PACKET_SIZE];
	char recvBuffer[PACKET_SIZE];
	WSABUF buff;
	size_t bytesSent;
	size_t bytesRecv;
	size_t contentSize;
	
	// Endpoint identifier
	std::string id;

	// Client socket instance
	SOCKET socket;

private:
	// Packet processing resources
	size_t dataSize_;
	std::string messageBuffer_;
};