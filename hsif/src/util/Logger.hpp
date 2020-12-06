#pragma once

#include <fstream>
#include <chrono>
#include <iostream>
#include <ctime>

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
/// Encapsulates logging functionality for console and file-based logging.
/// </summary>
class Logger
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Logger() : logFile_(std::ofstream("default_log.txt", std::ofstream::out)) {};

	/// <summary>
	/// Constructor with custom filepath.
	/// </summary>
	/// <param name="filePath">Specified path to file to log to.</param>
	Logger(std::string filePath) : logFile_(std::ofstream(filePath, std::ofstream::out)) {};

	/// Default destructor.
	~Logger() { logFile_.close(); };

	/// <summary>
	/// Logs message to console and file if specified.
	/// </summary>
	/// <param name="message">String message to log.</param>
	/// <param name="toFile">Designates whether or not to log message to file.</param>
	void logMsg(std::string message, bool toFile)
	{
		// Generate current time
		auto rawTime = std::chrono::system_clock::now();
		auto convertTime = std::chrono::system_clock::to_time_t(rawTime);
		// Create formatted message with timestamp
		auto formattedMsg = "HSIF>> " + message + " - time: " + std::ctime(&convertTime);
		// Write to console
		std::cout << formattedMsg << std::endl;
		// Write to file if specified
		if (toFile)
			logFile_ << formattedMsg;
	};

private:
	// Log file object
	std::ofstream logFile_;
};