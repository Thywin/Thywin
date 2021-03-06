/*
 * Communicator.cpp
 *
 *  Created on: May 13, 2014
 *      Author: Thomas Kooi
 *      Author: Imre Woudstra
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
#include <system_error>

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
			throw std::system_error();
		}

		if (connect(connectionSocket, (struct sockaddr*) &sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::system_error();
		}
	}

	Communicator::~Communicator()
	{
		close(connectionSocket);
		//If close fails it doesn't matter.
	}

	unsigned int Communicator::SendPacket(const ThywinPacket& packet)
	{
		std::stringstream data;
		data << packet.Method << TP_HEADER_SEPERATOR << packet.Type << TP_HEADER_SEPERATOR;
		if (packet.Content != NULL)
		{
			data << packet.Content->Serialize();
		}
		data << TP_END_OF_PACKET;

		const char* sendBuffer = data.str().c_str();
		unsigned int totalBytesSent = 0;
		while (totalBytesSent < data.str().size())
		{
			int bytesSent = send(connectionSocket, &sendBuffer[totalBytesSent], data.str().size() - totalBytesSent, 0);

			if (bytesSent < 0)
			{
				throw std::system_error();
			}

			totalBytesSent += bytesSent;
		}

		return totalBytesSent;
	}

	std::shared_ptr<ThywinPacket> Communicator::ReceivePacket(std::shared_ptr<ThywinPacketContent> contentObject)
	{
		std::stringstream receiveStream;
		char receiveBuffer = 0;
		int receiveSize = 0;
		do
		{
			receiveSize = recv(connectionSocket, &receiveBuffer, sizeof(char), 0);
			checkReceivedPacketForError(receiveSize, connectionSocket);
			receiveStream << receiveBuffer;
		} while (receiveSize > 0 && receiveBuffer != TP_END_OF_PACKET);

		std::string extractedValueMethod;
		std::getline(receiveStream, extractedValueMethod, TP_HEADER_SEPERATOR);

		std::string extractedValueType;
		std::getline(receiveStream, extractedValueType, TP_HEADER_SEPERATOR);

		if (contentObject != NULL)
		{
			std::string extractedValueContent;
			std::getline(receiveStream, extractedValueContent, TP_END_OF_PACKET);
			contentObject->Deserialize(extractedValueContent);
		}

		std::shared_ptr<ThywinPacket> packet(
				new ThywinPacket((PacketMethod) std::stoi(extractedValueMethod),
						(PacketType) std::stoi(extractedValueType), contentObject));

		return packet;
	}

	void Communicator::checkReceivedPacketForError(int receiveSize, int socket)
	{
		if (receiveSize < 0)
		{
			throw std::system_error();
		}
		else if (receiveSize == 0)
		{
			if (close(socket) < 0)
			{
				throw std::system_error();
			}
			//throw an exception because when the master closes there is no use in continuing. 
			throw std::runtime_error(std::string("Master closed the connection"));
		}
		//no else because the code just continues the previous if's terminate the program.
	}

} /* namespace thywin */
