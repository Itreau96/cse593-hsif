#include "Endpoint.hpp"

bool Endpoint::connect()
{
    // Create a socket (IPv4, TCP)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cout << "Failed to create socket. errno: " << errno << std::endl;
        return false;
    }

    // Listen to port 9999 on any address
    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip_);
    sockaddr.sin_port = htons(port_); // htons is necessary to convert a number to
                                     // network byte order
    if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
        return false;
    }

    // Start listening. Hold at most 10 connections in the queue
    if (listen(sockfd, 10) < 0) {
        std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
        return false;
    }

    // Grab a connection from the queue
    auto addrlen = sizeof(sockaddr);
    int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
    if (connection < 0) {
        std::cout << "Failed to grab connection. errno: " << errno << std::endl;
        return false;
    }
}

void* Endpoint::readData()
{
    // Read from the connection
    char buffer[bufferSize_];
    auto bytesRead = read(connection, buffer, 100);
    return bytesRead;
}

bool Endpoint::writeData(void* someData)
{
    // Send a message to the connection
    send(connection, (char*)someData, bufferSize_, 0);
}

void Endpoint::disconnect()
{
    close(connection_);
    close(sockfd_);
}