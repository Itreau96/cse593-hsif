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
    contentSize(0),
    msgBytesRecv_(0)
{}

bool Endpoint::processRecv()
{
    // If room in buffer, append string delimiter to char array
    if (bytesRecv < PACKET_SIZE)
        recvBuffer[bytesRecv] = '\0';
    // Convert char array to string for simpler processing
    std::string stringBuffer(recvBuffer);
    // If dataSize not set, we are receiving a new message
    if (dataSize_ == 0)
    {
        // First check if string buffer has size delimiter (always has to have if new message)
        if (stringBuffer.find('\r') == std::string::npos)
            return false;
        else
        {
            // While string contains delimiter, process incoming messages
            while (stringBuffer.find('\r') != std::string::npos)
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
                    // If partition is size of message, process and clear buffer
                    if (partitions[1].length() < dataSize_)
                    {
                        messageBuffer_ += partitions[1];
                        msgBytesRecv_ += sizeof(partitions[1]);
                        stringBuffer = "";
                    }
                    // If partition doesn't contain full message, add to buffer and clear packet resources
                    else
                    {
                        std::string message = partitions[1].substr(0, dataSize_);
                        stringBuffer = partitions[1].substr(dataSize_);
                        outbox.push(message);
                        dataSize_ = 0;
                        msgBytesRecv_ = 0;
                    }
                }
            }
        }
    }
    // If size is known, we are still processing previous message
    else
    {
        // If multiple messages in buffer, process all packets
        if (bytesRecv > dataSize_ - msgBytesRecv_)
        {
            // Exctract remainder of message and append to outbox
            std::string remainingData = stringBuffer.substr(0, dataSize_ - msgBytesRecv_);
            std::string message = messageBuffer_ + remainingData;
            outbox.push(message);
            // Begin processing remainder of messages
            stringBuffer = stringBuffer.substr(dataSize_ - msgBytesRecv_);
            dataSize_ = 0;
            msgBytesRecv_ = 0;

            // While messages in packet, extract and send to outbox
            while (stringBuffer.find('\r') != std::string::npos)
            {
                std::vector<std::string> partitions;
                // Split header from payload
                partitions.push_back(stringBuffer.substr(0, stringBuffer.find('\r')));
                partitions.push_back(stringBuffer.substr(stringBuffer.find('\r') + 1));

                // Process payload size
                dataSize_ = std::stoi(partitions[0]);
                // If payload exists in packet, process
                if (partitions[1] != "")
                {
                    // If full payload not in packet, add to buffer and continue
                    if (partitions[1].length() < dataSize_)
                    {
                        messageBuffer_ += partitions[1];
                        msgBytesRecv_ += sizeof(partitions[1]);
                        stringBuffer = "";
                    }
                    // If payload in packet, extract and continue
                    else
                    {
                        std::string message = partitions[1].substr(0, dataSize_);
                        stringBuffer = partitions[1].substr(dataSize_);
                        outbox.push(message);
                        dataSize_ = 0;
                        msgBytesRecv_ = 0;
                    }
                }
            }
        }
        // If payload only in packet, process and continue
        else if (stringBuffer.length() + msgBytesRecv_ == dataSize_)
        {
            std::string message = messageBuffer_ + stringBuffer;
            outbox.push(message);
            dataSize_ = 0;
            messageBuffer_ = "";
            msgBytesRecv_ = 0;
;        }
        // If full payload not in packet, add to buffer and continue 
        else
        {
            messageBuffer_ += stringBuffer;
            msgBytesRecv_ += sizeof(stringBuffer);
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