#include "Server.hpp"

Server::Server(std::string ip, unsigned int port, DataReceiveCallback callback)
{
    WSADATA wsaData;
    int iResult;

    serverSocket_ = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Create a SOCKET for connecting to server
    serverSocket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (serverSocket_ == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = bind(serverSocket_, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(serverSocket_);
        WSACleanup();
        return;
    }

    do
    {
        iResult = listen(serverSocket_, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(serverSocket_);
            WSACleanup();
            return;
        }

        // Accept a client socket
        SOCKET clientSocket = accept(serverSocket_, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(serverSocket_);
            WSACleanup();
            return;
        }
        addEndpoint("test", clientSocket, callback); // TODO need to put variable size here...
    } while (iResult > 0);

    // Cleanup socket
    cleanup();
}

bool Server::addEndpoint(std::string id, SOCKET clientSocket, DataReceiveCallback callback)
{
	if (endpoints_.find(id) == endpoints_.end())
	{
		return false;
	}

	auto endpoint = std::make_shared<Endpoint>(clientSocket, DEFAULT_BUFLEN, callback); // TODO: add variable size here
	endpoints_[id] = endpoint;

	return true;
}

bool Server::sendData(std::string id, MsgDataPtr msg)
{
    return true;
}

bool Server::removeEndpoint(std::string id)
{
	if (endpoints_.find(id) == endpoints_.end())
	{
		endpoints_.erase(id);
		return true;
	}

	return false;
}

std::shared_ptr<Endpoint> Server::getEndpoint(std::string id)
{
	if (endpoints_.find(id) == endpoints_.end())
	{
		return endpoints_[id];
	}

	return nullptr;
}

void Server::cleanup()
{
    // Loop through endpoints and cleanup? Or let destructor handle it...
    closesocket(serverSocket_);
    WSACleanup();
}