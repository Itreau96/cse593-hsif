#include "Endpoint.hpp"

Endpoint::Endpoint(SOCKET clientSocket, unsigned int bufferSize, DataReceiveCallback callback) :
    clientSocket_(clientSocket),
    bufferSize_(bufferSize),
    callback_(callback),
    valid_(true)
{
    int iSendResult;
    recvbuflen_ = bufferSize;

    // Receive until the peer shuts down the connection
    do {
        iResult_ = recv(clientSocket_, recvbuf_, recvbuflen_, 0);
        if (iResult_ > 0) {
            printf("Bytes received: %d\n", iResult_);
            MsgDataPtr ptr = std::make_shared<MsgData>(recvbuf_, recvbuflen_, "me", "you");
            callback(ptr);

            char* buffer = "ack";
            int iSendResult = send(clientSocket_, buffer, sizeof(buffer), 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                cleanup();
                return;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult_ == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            cleanup();
            return;
        }

    } while (iResult_ > 0);

    cleanup();
}

bool Endpoint::sendData(const std::any& data, size_t dataSize)
{
    char* buffer = std::any_cast<char*>(data);
    int iSendResult = send(clientSocket_, buffer, dataSize, 0);
    if (iSendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(clientSocket_);
        WSACleanup();
        return false;
    }
    printf("Bytes sent: %d\n", iSendResult);

    return true;
}

bool Endpoint::valid()
{
    return valid_;
}

void Endpoint::cleanup()
{
    // shutdown the connection since we're done
    iResult_ = shutdown(clientSocket_, SD_SEND);
    if (iResult_ == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(clientSocket_);
        return;
    }

    // cleanup
    closesocket(clientSocket_);
    valid_ = false;
}