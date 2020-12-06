#include "Logger.hpp"
#include "gtest/gtest.h"
#include <filesystem>

// Test logger default initialization
TEST(LoggerTest, TestInit)
{
	// Assert true if default log file is generated
	auto logger = Logger();
	const std::filesystem::path logPath = std::filesystem::current_path() / "default_log.txt";
	ASSERT_TRUE(std::filesystem::exists(logPath));
}

// Test constructor with file
TEST(LoggerTest, TestInitWithFile)
{
	// Assert true if custom file generated successfully
	auto logger = Logger("test_log_file.txt");
	const std::filesystem::path logPath = std::filesystem::current_path() / "test_log_file.txt";
	ASSERT_TRUE(std::filesystem::exists(logPath));
}

// Test log without file
TEST(LoggerTest, TestLogConsole)
{
	// Assert true if file empty
	auto logger = new Logger("test_empty_log.txt");
	const std::filesystem::path logPath = std::filesystem::current_path() / "test_empty_log.txt";
	ASSERT_TRUE(std::filesystem::exists(logPath));
	// Log to console and file
	logger->logMsg("This message should just go to console!", false);
	// Delete logger so file closes
	delete logger;
	ASSERT_TRUE(std::filesystem::is_empty(logPath));
}

// Test log with file
TEST(LoggerTest, TestLogFile)
{
	// Assert true if file not empty
	auto logger = new Logger("test_populated_log.txt");
	const std::filesystem::path logPath = std::filesystem::current_path() / "test_populated_log.txt";
	ASSERT_TRUE(std::filesystem::exists(logPath));
	// Log to console and file
	logger->logMsg("This message should go to file too!", true);
	// Delete logger so file closes
	delete logger;
	ASSERT_FALSE(std::filesystem::is_empty(logPath));
}