#pragma once

#include "Endpoint.hpp"
#include <vector>
#include <unordered_map>
#include <memory>

using EndpointMap = std::unordered_map<std::string, std::shared_ptr<Endpoint>>;

class Server
{
public:
	Server() = default;

	~Server() = default;

	bool addEndpoint(std::string id, std::string ip, unsigned int port);

	bool removeEndpoint(std::string id);

	std::shared_ptr<Endpoint> getEndpoint(std::string id);

private:
	EndpointMap endpoints_;
};