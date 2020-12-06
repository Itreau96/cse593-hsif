#include "Server.hpp"
#include <iostream>
#include <thread>

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

#define DEFAULT_PORT 8888
#define DATA_BUFSIZE 1024

Server::Server() :
    ip_(""),
    port_(DEFAULT_PORT),
    logger_(std::make_shared<Logger>()),
    logToFile_(false),
    listenSocket_(INVALID_SOCKET)
{}

Server::Server(std::string ip, unsigned int port, std::shared_ptr<Logger> logger, bool logToFile) :
    ip_(ip),
    port_(port),
    logger_(logger),
    logToFile_(logToFile),
    listenSocket_(INVALID_SOCKET)
{}

bool Server::connect()
{
    // Initialize WSA library
    WSADATA wsaData;
    int wsaRet;
    if ((wsaRet = (WSAStartup(0x0202, &wsaData))) != 0)
    {
        logger_->logMsg("WSAStartup() failed with error: " + wsaRet, logToFile_);
        WSACleanup();
        return false;
    }
    else
        logger_->logMsg("WSAStartup() successful", logToFile_);

    // Initialize server socket
    if ((listenSocket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        logger_->logMsg("WSASocket() failed with error: " + WSAGetLastError(), logToFile_);
        WSACleanup();
        return false;
    }
    else
        logger_->logMsg("Listener socket initialization successful", logToFile_);

    // Create socket information object
    SOCKADDR_IN InternetAddr;
    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(port_);

    // Bind socket to given address/port
    if (bind(listenSocket_, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
    {
        logger_->logMsg("bind() failed with error: " + WSAGetLastError(), logToFile_);
        return false;
    }
    else
        logger_->logMsg("Socket bind successful", logToFile_);

    // Initialize listener on socket
    if (listen(listenSocket_, 5))
    {
        logger_->logMsg("listen() failed with error: " + WSAGetLastError(), logToFile_);
        return false;
    }
    else
        logger_->logMsg("listen() on port " + std::to_string(port_) + " successful", logToFile_);

    // Set non-blocking
    ULONG nonBlock = 1;
    if (ioctlsocket(listenSocket_, FIONBIO, &nonBlock) == SOCKET_ERROR)
    {
        logger_->logMsg("Ioctlsocket() failed with error: " + WSAGetLastError(), logToFile_);
        return false;
    }
    else
        logger_->logMsg("Non-Blocking listener setup successful", logToFile_);

    // Return successful initialization
    return true;
}

bool Server::poll()
{
    // Initialize local variables
    int total;
    SOCKET acceptSocket = INVALID_SOCKET;
    DWORD sendBytes;
    DWORD recvBytes;
    FD_SET readSet;
    FD_SET writeSet;

    // Prepare the Read and Write socket sets for network I/O notification
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);

    // Always allow reading from sockets
    FD_SET(listenSocket_, &readSet);

    // If data available in endpoint outboxes, add to write set
    for (const std::shared_ptr<Endpoint>& endpoint : endpoints_)
    {
        if (endpoint->contentSize > 0)
            FD_SET(endpoint->socket, &writeSet);
        else
            FD_SET(endpoint->socket, &readSet);
    }

    // Block unless change on sockets
    if ((total = select(0, &readSet, &writeSet, NULL, NULL)) == SOCKET_ERROR)
    {
        logger_->logMsg("Select() returned with error: " + WSAGetLastError(), logToFile_);
        return false;
    }
    else
        logger_->logMsg("Poll on endpoint successful", logToFile_);

    // Check for new connections on listening socket
    if (FD_ISSET(listenSocket_, &readSet))
    {
        // Attempt to create endpoint from new socket
        total--;
        if ((acceptSocket = accept(listenSocket_, NULL, NULL)) != INVALID_SOCKET)
        {
            ULONG nonBlock = 1;
            if (ioctlsocket(acceptSocket, FIONBIO, &nonBlock) == SOCKET_ERROR)
            {
                logger_->logMsg("Ioctlsocket(FIONBIO) failed with error: " + WSAGetLastError(), logToFile_);
                return false;
            }
            else
                logger_->logMsg("Non-blocking client socket initialized", logToFile_);

            // Create endpoint
            if (createEndpoint(acceptSocket) == false)
            {
                logger_->logMsg("CreateSocketInformation(AcceptSocket) failed", logToFile_);
                return false;
            }
            else
                logger_->logMsg("Endpoint created successfully!", logToFile_);
        }
        // If blocking exception, catch
        else
        {
            // Log error
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                logger_->logMsg("accept() failed with error: " + WSAGetLastError(), logToFile_);
                return false;
            }
            // Continue if accept
            else
                logger_->logMsg("Accept successful", logToFile_);
        }
    }

    // Loop through each endpoint in the vector (using for loop so we can use index)
    for (size_t index = 0; index < endpoints_.size(); index++)
    {
        // If flagged for reading incoming data, process incoming packet
        if (FD_ISSET(endpoints_[index]->socket, &readSet))
        {
            total--;

            // Set WSA buffer values
            endpoints_[index]->buff.buf = endpoints_[index]->recvBuffer;
            endpoints_[index]->buff.len = 1024;

            // Write data to buffer
            DWORD flags = 0;
            if (WSARecv(endpoints_[index]->socket, &(endpoints_[index]->buff), 1, &recvBytes, &flags, NULL, NULL) == SOCKET_ERROR)
            {
                // If blocking exception, delete endpoint
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    logger_->logMsg("WSARecv() failed with error: " + WSAGetLastError(), logToFile_);
                    deleteEndpoint(index);
                }
                // Otherwise continue...
                else
                    logger_->logMsg("Packet received from endpoint successfully", logToFile_);
                continue;
            }
            else
            {
                // Log bytes recieved
                endpoints_[index]->bytesRecv = recvBytes;

                // Process packet recieved
                bool success = endpoints_[index]->processRecv();

                // If zero bytes are received, or unsuccessful parsing, close the connection
                if (recvBytes == 0 || !success)
                {
                    deleteEndpoint(index);
                    continue;
                }
            }
        }

        // Check for outgoing messages
        int size = endpoints_[index]->outbox.size();
        // While messages available for routing, route and continue.
        while (size > 0)
        {
            std::string currentMsg = endpoints_[index]->outbox.front();
            endpoints_[index]->outbox.pop();
            routeMessage(currentMsg, index);
            size--;
        }

        // If messages available to write, send packet
        if (FD_ISSET(endpoints_[index]->socket, &writeSet))
        {
            total--;

            // Set WSA buffer values
            endpoints_[index]->buff.buf = endpoints_[index]->sendBuffer + endpoints_[index]->bytesSent;
            endpoints_[index]->buff.len = endpoints_[index]->contentSize - endpoints_[index]->bytesSent;

            // Write data to outgoing socket
            if (WSASend(endpoints_[index]->socket, &(endpoints_[index]->buff), 1, &sendBytes, 0, NULL, NULL) == SOCKET_ERROR)
            {
                // If blocking exception, log and remove endpoint
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    logger_->logMsg("WSASend() failed with error: " + WSAGetLastError(), logToFile_);
                    deleteEndpoint(index);
                }
                // Otherwise, continue
                else
                    logger_->logMsg("Packet sent from endpoint successfully", logToFile_);
                continue;
            }
            else
            {
                // Log sent bytes
                endpoints_[index]->bytesSent += sendBytes;
                    
                // If all bytes sent, clear buffer
                if (endpoints_[index]->bytesSent == endpoints_[index]->contentSize)
                    endpoints_[index]->bufferReady();
            }
        }
    }
}

bool Server::createEndpoint(SOCKET clientSocket)
{
    // Create shared pointer to endpoint and add to endpoint buffer
    auto socketInfo = std::make_shared<Endpoint>(clientSocket);
    endpoints_.push_back(socketInfo);
    return true;
}

bool Server::registerEndpoint(std::string id, size_t index)
{
    // Attempt to register endpoint with identifier
    auto ret = endpointMap_.insert({ id, index });
    // If failure, return
    if (ret.second == false)
    {
        return false;
    }
    // If successful, set endpoint id value and log success message
    else
    {
        endpoints_[index]->id = id;
        logger_->logMsg("Endpoint registered successfully: " + id, logToFile_);
        return true;
    }
}

void Server::routeMessage(std::string msg, size_t index)
{
    // Parse message
    json message = json::parse(msg);
    // If registration message, attempt to register
    if (message.contains(std::string("type")) && message["type"] == "registration")
    {
        registerEndpoint(message["value"], index);
    }
    // If basic message, attempt to send to designated endpoint
    else if (message.contains(std::string("type")) && message["type"] == "message")
    {
        auto recvEndpoint = getEndpoint(message["to"]);
        // If endpoint found, log success and add to message inbox
        if (recvEndpoint)
        {
            logger_->logMsg("Sending message: " + message.dump(), logToFile_);
            recvEndpoint->receiveMsg(message.dump());
        }
        // If endpoint not found (possibly due to endpoint not registered yet), push message back into outbox
        else
        {
            endpoints_[index]->outbox.push(msg);
        }
    }
    else
    {
        // Future message types will be handled here
    }
}

bool Server::deleteEndpoint(size_t index)
{
    // Dispose of socket connection
    auto endPoint = endpoints_[index];
    endPoint->cleanup();
    // Remove endpoint reference from map and list
    endpoints_.erase(endpoints_.begin() + index);
    endpointMap_.erase(endPoint->id);
    return true;
}

std::shared_ptr<Endpoint> Server::getEndpoint(std::string id)
{
    // If endpoint exists in map, return
	if (endpointMap_.find(id) != endpointMap_.end())
	{
		return endpoints_[endpointMap_[id]];
	}

    // Return null if not found
	return nullptr;
}

size_t Server::numEndpoints()
{
    return endpoints_.size();
}

size_t Server::numRegisteredEndpoints()
{
    return endpointMap_.size();
}

void Server::cleanup()
{
    // Dispose of socket resources
    closesocket(listenSocket_);
    WSACleanup();
}