#pragma once

#include "Server.hpp"
#include "Logger.hpp"

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

/// <summary>
/// Instantiates default configuration of HSIF server.
/// </summary>
class Sys
{
public:
	// Delete default constructor
	Sys() = delete;

	/// <summary>
	/// Constructor for system given server and logger parameters.
	/// </summary>
	/// <param name="ip"></param>
	/// <param name="port"></param>
	/// <param name="logToFile"></param>
	Sys(std::string ip, unsigned int port, bool logToFile);

	/// <summary>
	/// Constructor for system given server and logger parameters.
	/// </summary>
	/// <param name="ip">Ip to listen for incoming connections.</param>
	/// <param name="port">Port to listen for incoming connections.</param>
	/// <param name="logFile">Specified filepath for log.</param>
	/// <param name="logToFile">Determines whether or not to log to file.</param>
	Sys(std::string ip, unsigned int port, std::string logFile, bool logToFile);
private:
	// Binds server instance to provided ip and port
	void startServer();
	// HSIF server instance
	Server server_;
	// HSIF logger shared pointer instance
	std::shared_ptr<Logger> logger_;
};