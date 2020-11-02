#pragma once

#include "DataManager.hpp"
#include "Server.hpp"

class Sys
{
public:
	Sys();
	void startServer(std::string ip, unsigned int port);
	void stopServer();
	void clearData();
private:
	DataManager dataMgr_;
	Server server_;
};