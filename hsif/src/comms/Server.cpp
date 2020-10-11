#include "Server.hpp"

bool Server::addEndpoint(std::string id, std::string ip, unsigned int port)
{
	if (endpoints_.find(id) == endpoints_.end())
	{
		return false;
	}

	auto endpoint = std::make_shared<Endpoint>(ip, port);
	endpoints_[id] = endpoint;

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