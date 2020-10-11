#pragma once

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string> 

class Endpoint
{
public:
	Endpoint() : ip_(""), port_(-1), active_(false) {}

	Endpoint(std::string ip, unsigned int port) : ip_(ip), port_(port), active_(false) {}

	~Endpoint() = default;

	void setIp(std::string ip)
	{
		ip_ = ip;
	}

	const std::string getIp()
	{
		return ip_;
	}

	void setPort(unsigned int port)
	{
		port_ = port;
	}

	const unsigned int getPort()
	{
		return port_;
	}

	void setBufferSize(size_t bufferSize)
	{
		bufferSize_ = bufferSize;
	}

	const size_t getBufferSize()
	{
		return bufferSize_;
	}

	void 

	bool connect();

	void disconnect();

	void* readData(); // Use message data here

	bool writeData(void* someData); // Use message data here

private:
	std::string ip_;
	unsigned int port_;
	bool active_;
	size_t bufferSize_;

	int sockfd_;
	int connection_;
};