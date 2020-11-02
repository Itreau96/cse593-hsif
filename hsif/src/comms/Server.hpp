#pragma once

#include "Endpoint.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <any>

using EndpointMap = std::unordered_map<std::string, std::shared_ptr<Endpoint>>;

class Server
{
public:

	Server(std::string ip, unsigned int port, DataReceiveCallback callback);

	~Server() = default;

	bool addEndpoint(std::string id, SOCKET clientSocket, DataReceiveCallback callback);

	bool removeEndpoint(std::string id);

	std::shared_ptr<Endpoint> getEndpoint(std::string id);

	bool sendData(std::string id, MsgDataPtr msg);

	void cleanup();

private:
	SOCKET serverSocket_;
	EndpointMap endpoints_;
};

