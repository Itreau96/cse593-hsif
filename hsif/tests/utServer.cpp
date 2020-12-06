#include "Server.hpp"
#include <Windows.h>
#include "gtest/gtest.h"
#include <thread>
#include <future>

#define TEST_BUFLEN 1024

// Testing function for generating a dummy endpoint
void createTestEndpoint(std::string ip, unsigned int port, std::string id, std::string msg, std::promise<std::string> && p)
{
    // Socket variables
    WSADATA wsaData;
    char recvbuf[TEST_BUFLEN];
    int iResult;
    int recvBufLen = TEST_BUFLEN;
    SOCKET testSocket;
    SOCKADDR_IN serverAddr, senderInfo;
    int retCode;
    int bytesSent, bytesRecv, nlen;
    // Message variables
    const std::string registration = "{ \"type\" : \"registration\", \"value\" : \"" + id + "\"  }";
    std::string registrationMsg = std::to_string(registration.length()) + '\r' + registration;
    std::string jsonMsg = std::to_string(msg.length()) + '\r' + msg;

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Bind socket
    testSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Set server address info
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // Make a connection to the HSIF server
    connect(testSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    // Send registration and test message
    send(testSocket, registrationMsg.c_str(), registrationMsg.length(), 0);
    bytesSent = send(testSocket, jsonMsg.c_str(), jsonMsg.length(), 0);

    // Receive message from opposing endpoint
    bytesRecv = recv(testSocket, recvbuf, recvBufLen, 0);

    // Null terminate string
    if (bytesRecv < recvBufLen)
        recvbuf[bytesRecv] = '\0';
    // Set return value to message received
    p.set_value(std::string(recvbuf));
}

// Test endpoint initialization
TEST(ServerTest, TestInit)
{
    auto server = std::make_shared<Server>();
    ASSERT_TRUE(server != nullptr);
}

// Test connect
TEST(ServerTest, TestConnect)
{
    auto logger = std::make_shared<Logger>();
    auto server = Server("", 7777, logger, false);
    bool connectSuccess = server.connect();
    ASSERT_TRUE(connectSuccess);
}

// Test add endpoint
TEST(ServerTest, TestAddEndpoint)
{
    SOCKET testSock = INVALID_SOCKET;
    auto logger = std::make_shared<Logger>();
    auto server = Server("", 7777, logger, false);
    server.createEndpoint(testSock);
    ASSERT_TRUE(server.numEndpoints() == 1);
}

// Test delete endpoint
TEST(ServerTest, TestDeleteEndpoint)
{
    SOCKET testSock = INVALID_SOCKET;
    auto logger = std::make_shared<Logger>();
    auto server = Server("", 7777, logger, false);
    server.createEndpoint(testSock);
    server.deleteEndpoint(0);
    ASSERT_TRUE(server.numEndpoints() == 0);
}

// Test register endpoint
TEST(ServerTest, TestRegisterEndpoint)
{
    SOCKET testSock = INVALID_SOCKET;
    auto logger = std::make_shared<Logger>();
    auto server = Server("", 7777, logger, false);
    server.createEndpoint(testSock);
    bool success = server.registerEndpoint("testId", 0);
    ASSERT_TRUE(success);
}

// Test get endpoint
TEST(ServerTest, TestGetEndpoint)
{
    SOCKET testSock = INVALID_SOCKET;
    auto logger = std::make_shared<Logger>();
    auto server = Server("", 7777, logger, false);
    server.createEndpoint(testSock);
    server.registerEndpoint("testId", 0);
    auto returnedEndpoint = server.getEndpoint("testId");
    ASSERT_TRUE(returnedEndpoint->socket == testSock);
    ASSERT_TRUE(returnedEndpoint->id == "testId");
}

// Test messaging routing
TEST(ServerTest, TestRouting)
{
    SOCKET testSock = INVALID_SOCKET;
    std::string regMsg = "{\"type\":\"registration\",\"value\":\"testId\"}";
    std::string msg = "{\"type\":\"message\",\"from\":\"testId\",\"to\":\"testId\",\"data\":{\"data\":\"testData\"}}";
    std::string finalMsg = std::to_string(msg.length()) + '\r' + msg;

    auto logger = std::make_shared<Logger>();
    auto server = Server("", 7777, logger, false);
    server.createEndpoint(testSock);
    server.routeMessage(regMsg, 0);
    server.routeMessage(msg, 0);
    std::shared_ptr<Endpoint> endpoint = server.getEndpoint("testId");
    ASSERT_TRUE(endpoint != nullptr);
    ASSERT_TRUE(finalMsg.size() == endpoint->contentSize);
}

// Test endpoint polling from two separate 
TEST(ServerTest, TestEndpointPoll)
{
    auto logger = std::make_shared<Logger>();
    auto server = Server("127.0.0.1", 7777, logger, false);
    bool success = server.connect();

    std::promise<std::string> p;
    auto f = p.get_future();
    std::promise<std::string> p2;
    auto f2 = p2.get_future();

    std::thread ep1(&createTestEndpoint, "127.0.0.1", 7777, "testId", "{ \"type\" : \"message\", \"from\" : \"testId\", \"to\" : \"otherTestId\", \"data\": { \"data\" :\"testData\" } }", std::move(p));
    std::thread ep2(&createTestEndpoint, "127.0.0.1", 7777, "otherTestId", "{ \"type\" : \"message\", \"from\" : \"otherTestId\", \"to\" : \"testId\", \"data\": { \"data\" :\"otherTestData\" } }", std::move(p2));
    // Poll a few times. Polling executes small steps to be more performant. Poll multiple times to get to point where both endpoints have received messages.
    success = server.poll();
    success = server.poll();
    success = server.poll();
    success = server.poll();
    success = server.poll();
    ep2.join();
    ep1.join();
    auto data1 = f.get();
    auto data2 = f2.get();
    ASSERT_TRUE(success);
    ASSERT_TRUE(data1 == "85\r{\"data\":{\"data\":\"otherTestData\"},\"from\":\"otherTestId\",\"to\":\"testId\",\"type\":\"message\"}");
    ASSERT_TRUE(data2 == "80\r{\"data\":{\"data\":\"testData\"},\"from\":\"testId\",\"to\":\"otherTestId\",\"type\":\"message\"}");
}
