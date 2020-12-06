#pragma once

#include "Endpoint.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <any>
#include <mutex>
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

using EndpointMap = std::unordered_map<std::string, size_t>;

/// <summary>
/// HSIF server class. Orchestrates endpoint registration and communication.
/// </summary>
class Server
{
public:
	/// <summary>
	/// Default initialization
	/// </summary>
	Server();

	/// <summary>
	/// Server initialization with socket and logging parameters.
	/// </summary>
	/// <param name="ip">Ip address to accept connections. Provide "" to accept connections on any ip.</param>
	/// <param name="port">TCP port to listen for new connections and data.</param>
	/// <param name="logger">Pointer reference to Logger utility object.</param>
	/// <param name="logToFile">Determines whether or not to log to file.</param>
	Server(std::string ip, unsigned int port, std::shared_ptr<Logger> logger, bool logToFile);

	~Server() = default;

	/// <summary>
	/// Initializes server socket connection.
	/// </summary>
	/// <returns>Returns whether or not connection was successful.</returns>
	bool connect();

	/// <summary>
	/// Poll function. Executes single select() call and performs socket IO on endpoints.
	/// </summary>
	/// <returns>Returns false if exception occurred during poll.</returns>
	bool poll();

	/// <summary>
	/// Creates an endpoint given a new socket connection.
	/// </summary>
	/// <param name="clientSocket">Socket client created from incoming connection to server.</param>
	/// <returns>Returns true if creation successful.</returns>
	bool createEndpoint(SOCKET clientSocket);

	/// <summary>
	/// Registers endpoint with provided identification string.
	/// </summary>
	/// <param name="id">Identifying string for sending and receiving messages.</param>
	/// <param name="index">Index of endpoint in endpoints vector.</param>
	/// <returns>True if registration successful.</returns>
	bool registerEndpoint(std::string id, size_t index);

	/// <summary>
	/// Routes message provided based on identifying "to" information.
	/// </summary>
	/// <param name="msg">JSON formatted string message.</param>
	/// <param name="index">Index of endpoint in vector.</param>
	void routeMessage(std::string msg, size_t index);

	/// <summary>
	/// Removes an endpoint given its index in the endpoints vector. 
	/// </summary>
	/// <param name="index">Index of endpoint in vector.</param>
	/// <returns>Returns true if successful.</returns>
	bool deleteEndpoint(size_t index);

	/// <summary>
	/// Returns an endpoint based on the string identifier.
	/// </summary>
	/// <param name="id">String identifier for the endpoint.</param>
	/// <returns>Pointer to endpoint instance.</returns>
	std::shared_ptr<Endpoint> getEndpoint(std::string id);

	/// <summary>
	/// Closes server and client socket connections.
	/// </summary>
	void cleanup();

	/// <summary>
	/// Helper function used to determine number of endpoints connected to server.
	/// </summary>
	/// <returns>Integer value representing endpoint vector size.</returns>
	size_t numEndpoints();

	/// <summary>
	/// Helper function used to determine number of endpoints with registered identifiers.
	/// </summary>
	/// <returns>Integer value representing number of registered endpoints.</returns>
	size_t numRegisteredEndpoints();

private:
	// Socket for listening for incoming connections
	SOCKET listenSocket_;
	// Port for listening to incoming connections
	unsigned int port_;
	// Ip to listen for incoming connections ("" if accept from anywhere)
	std::string ip_;
	// List of endpoint instances currently connected to server
	std::vector<std::shared_ptr<Endpoint>> endpoints_;
	// Mapping between endpoint instances and their string identifiers
	EndpointMap endpointMap_;
	// Current logger object
	std::shared_ptr<Logger> logger_;
	// Determines whether or not to log to a file
	bool logToFile_;
};

