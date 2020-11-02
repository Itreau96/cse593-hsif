#include "../Endpoint.hpp"
#include "gtest/gtest.h"
#include <thread>

class TestClass
{
public:
    TestClass() = default;

    void someMemberFunction(MsgDataPtr data) { counter_++; }

    int getCount() { return counter_; }

private:
    int counter_ = 0;
};

void runEndpoint(SOCKET clientSocket, unsigned int buffLen, DataReceiveCallback receiveCallback)
{
    auto endPoint = Endpoint(clientSocket, DEFAULT_BUFLEN, receiveCallback);

    while (endPoint.valid())
    {
        // idle...
    }
}

void startConnection()
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("localhost", "6666", &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return;
    }

    // Send an initial buffer
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    printf("Bytes Sent from client: %ld\n", iResult);

    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);
    }

    // Send a second buffer
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    printf("Bytes Sent from client: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    // cleanup
    WSACleanup();
    closesocket(ConnectSocket);
}

// Test endpoint initialization
TEST(EndpointTest, TestInit) {
    WSADATA wsaData;
    int iResult;

    SOCKET serverSocket_ = INVALID_SOCKET;
    TestClass tester = TestClass();

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo("localhost", "6666", &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Create a SOCKET for connecting to server
    serverSocket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (serverSocket_ == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = bind(serverSocket_, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(serverSocket_);
        WSACleanup();
        return;
    }

    // Start connection thread
    std::thread clientThread(startConnection);

    iResult = listen(serverSocket_, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket_);
        WSACleanup();
        return;
    }

    // Accept a client socket
    SOCKET clientSocket = accept(serverSocket_, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket_);
        WSACleanup();
        return;
    }

    using std::placeholders::_1;
    DataReceiveCallback receiveCallback = std::bind(&TestClass::someMemberFunction, &tester, _1);
    std::thread endPt(runEndpoint, clientSocket, DEFAULT_BUFLEN, receiveCallback);

    clientThread.join();
    endPt.join();
    
    ASSERT_TRUE(tester.getCount() == 1);


    // Cleanup socket
    closesocket(serverSocket_);
    WSACleanup();
}

// Tests positive input.
TEST(EndpointTest, Positive) {

}