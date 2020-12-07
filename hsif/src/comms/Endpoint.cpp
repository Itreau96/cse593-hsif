#include "Endpoint.hpp"
#include <iostream>
#include <sstream>

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

Endpoint::Endpoint(SOCKET clientSocket) :
    socket(clientSocket),
    id(""),
    bytesRecv(0),
    bytesSent(0),
    dataSize_(0),
    contentSize(0)
{}

bool Endpoint::processRecv()
{
    // If room in buffer, append string delimiter to char array
    if (bytesRecv < PACKET_SIZE)
        recvBuffer[bytesRecv] = '\0';
    // Convert char array to string for simpler processing
    std::string stringBuffer(recvBuffer);
    // If dataSize not set, we are receiving a new message
    while (stringBuffer != "")
    {
        if (dataSize_ == 0)
        {
            // First check if string buffer has size delimiter (always has to have if new message)
            if (stringBuffer.find('\r') == std::string::npos)
                return false;
            else
            {
                std::vector<std::string> partitions;
                // Split header from payload
                partitions.push_back(stringBuffer.substr(0, stringBuffer.find('\r')));
                partitions.push_back(stringBuffer.substr(stringBuffer.find('\r') + 1));

                // Retrieve payload size
                dataSize_ = std::stoi(partitions[0]);
                // If payload exists in current packet, process
                if (partitions[1] != "")
                {
                    stringBuffer = partitions[1];
                }
            }
        }
        // If size is known, we are still processing previous message
        else
        {
            // Check if message contained in packet
            if (stringBuffer.length() >= dataSize_)
            {
                // Extract message
                std::string remainingData = stringBuffer.substr(0, dataSize_);
                std::string message = messageBuffer_ + remainingData;
                outbox.push(message);
                // Reset buffers and size variables
                stringBuffer = stringBuffer.substr(dataSize_);
                messageBuffer_ = "";
                dataSize_ = 0;
            }
            // If only part, subtract from data size and continue
            else
            {
                // Append to buffer and continue
                dataSize_ -= stringBuffer.length();
                messageBuffer_ += stringBuffer;
                stringBuffer = "";
            }
        }
    }
    // Clear recv buffer and recv byte counter
    memset(recvBuffer, 0, sizeof recvBuffer);
    bytesRecv = 0;
    return true;
}

void Endpoint::bufferReady()
{
    // Start by clearing buffer and size counters
    memset(sendBuffer, 0, PACKET_SIZE);
    bytesSent = 0;
    contentSize = 0;

    // If data in inbox, move to buffer
    if (inbox != "")
    {
        // If inbox larger than packet, extract packet from inbox 
        if (inbox.length() > PACKET_SIZE)
        {
            std::string packet = inbox.substr(0, PACKET_SIZE);
            strcpy(sendBuffer, packet.c_str());
            contentSize = sizeof sendBuffer;
            inbox = inbox.substr(contentSize);
        }
        // If smaller or equal to buffer, extract and clear inbox
        else
        {
            std::size_t length = inbox.copy(sendBuffer, inbox.length(), 0);
            sendBuffer[length] = '\0';
            contentSize = inbox.length();
            inbox = "";
        }
    }
}

void Endpoint::receiveMsg(std::string msg)
{
    // Generate HSIF message from JSON string input
    inbox += std::to_string(msg.length()) + '\r' + msg;
    // If no data in outgoing buffer, fill buffer with new message contents
    if (contentSize == 0)
    {
        bufferReady();
    }
}

void Endpoint::cleanup()
{
    // Close socket connection
    int result = shutdown(socket, SD_SEND);
    closesocket(socket);
}