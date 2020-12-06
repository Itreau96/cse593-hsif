#include "Endpoint.hpp"
#include "gtest/gtest.h"
#include <thread>

// Tests Endpoint initialization
TEST(TestEndpoint, TestInit)
{
	auto endpoint = std::make_shared<Endpoint>(INVALID_SOCKET);
	ASSERT_TRUE(endpoint != nullptr);
}

// Test receive message
TEST(TestEndpoint, TestRecvMsg)
{
	std::string testMsg = "{ \"TestData\" : \"TestValue\" }";
	auto endpoint = Endpoint(INVALID_SOCKET);
	std::string finalMsg = std::to_string(testMsg.length()) + '\r' + testMsg;
	endpoint.receiveMsg(testMsg);
	ASSERT_TRUE(endpoint.inbox == "");
	ASSERT_TRUE(endpoint.contentSize == finalMsg.length());
	ASSERT_TRUE(strncmp(endpoint.sendBuffer, finalMsg.c_str(), finalMsg.length()) == 0);
}

// Test clear buffer
TEST(TestEndpoint, TestClearBuffer)
{
	// Populate buffer
	std::string testMsg = "{ \"TestData\" : \"TestValue\" }";
	auto endpoint = Endpoint(INVALID_SOCKET);
	endpoint.receiveMsg(testMsg);
	// Clear buffer 
	endpoint.bufferReady();
	ASSERT_TRUE(endpoint.inbox == "");
	ASSERT_TRUE(endpoint.contentSize == 0);
	ASSERT_TRUE(endpoint.sendBuffer[0] == 0);
}

// Test endpoint's ability to parse messages
TEST(TestEndpoint, TestMessageRecv)
{
	std::string testMsg = "{ \"TestData\" : \"TestValue\" }";
	std::string finalMsg = std::to_string(testMsg.length()) + '\r' + testMsg;
	auto endpoint = Endpoint(INVALID_SOCKET);
	strcpy(endpoint.recvBuffer, finalMsg.c_str());
	endpoint.bytesRecv = finalMsg.length();
	endpoint.processRecv();

	ASSERT_TRUE(endpoint.outbox.front() == testMsg);
}

// Test endpoint cleanup
TEST(TestEndpoint, TestCleanup)
{

	auto endpoint = Endpoint(INVALID_SOCKET);
	endpoint.cleanup();
}