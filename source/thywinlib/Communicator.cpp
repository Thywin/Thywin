/*
 * Communicator.cpp
 *
 *  Created on: May 13, 2014
 *      Author: Thomas Kooi
 *      		Imre Woudstra
 */

#include "Communicator.h"

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

namespace thywin
{

	Communicator::Communicator(const std::string& ipaddress, const int& serverPort)
	{
		struct sockaddr_in sockAddr;
		sockAddr.sin_addr.s_addr = inet_addr(ipaddress.c_str());
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(serverPort);
		connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (connectionSocket == -1)
		{
			throw std::runtime_error(std::string("failed to create socket: ") + strerror(errno));
		}

		if (connect(connectionSocket, (struct sockaddr*) &sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::runtime_error(std::string("failed to connect to socket: ") + strerror(errno));
		}
	}

	Communicator::~Communicator()
	{
		if (close(connectionSocket) < 0)
		{
			throw std::runtime_error(std::string("failed to close socket: ") + strerror(errno));
		}
	}

	int Communicator::SendPacket(const ThywinPacket& packet)
	{
		std::stringstream data;
		data << packet.Method << TP_HEADER_SEPERATOR << packet.Type << TP_HEADER_SEPERATOR;
		if (packet.Content != NULL)
		{
			data << packet.Content->Serialize();
		}
		data << TP_END_OF_PACKET;

		int sendSize = send(connectionSocket, (char*) data.str().c_str(), data.str().size(), 0);
		if (sendSize < 0)
		{
			throw std::runtime_error(std::string("failed to send to socket: ") + strerror(errno));
		}

		return sendSize;
	}

	std::shared_ptr<ThywinPacket> Communicator::ReceivePacket(std::shared_ptr<ThywinPacketContent> contentObject)
	{
		std::shared_ptr<ThywinPacket> packet(new ThywinPacket);
		std::stringstream receiveStream;
		char receiveBuffer;
		int receiveSize;
		do
		{
			receiveSize = recv(connectionSocket, &receiveBuffer, sizeof(char), 0);
			receiveStream << receiveBuffer;
		} while (receiveSize > 0 && receiveBuffer != TP_END_OF_PACKET);

		if (receiveSize < 0)
		{
			throw std::runtime_error(std::string("failed to receive from socket: ") + strerror(errno));
		}
		else if (receiveSize == 0)
		{
			if (close(connectionSocket) < 0)
			{
				throw std::runtime_error(std::string("failed to close socket: ") + strerror(errno));
			}
			exit(EXIT_SUCCESS);
		}
		//no else because the code just continues the previous if's terminate the program.

		std::string extractedValue;
		std::getline(receiveStream, extractedValue, TP_HEADER_SEPERATOR);
		packet->Method = (PacketMethod) atoi(extractedValue.c_str());

		std::getline(receiveStream, extractedValue, TP_HEADER_SEPERATOR);
		packet->Type = (PacketType) atoi(extractedValue.c_str());

		std::getline(receiveStream, extractedValue, TP_END_OF_PACKET);
		contentObject->Deserialize(extractedValue);

		return packet;
	}
} /* namespace thywin */
