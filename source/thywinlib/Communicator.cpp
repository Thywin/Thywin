/*
 * Communicator.cpp
 *
 *  Created on: May 13, 2014
 *      Author: damonk
 */

#include "Communicator.h"

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

const char SEP = (char) 30;
const char EOT = (char) 4;

namespace thywin
{

	Communicator::Communicator(std::string ipaddress)
	{
		struct sockaddr_in sockAddr;
		sockAddr.sin_addr.s_addr = inet_addr(ipaddress.c_str());
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(7500);
		connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (connectionSocket == -1)
		{
			perror("Socket failed");
			exit(EXIT_FAILURE);
		}

		if (connect(connectionSocket, (struct sockaddr*) &sockAddr, sizeof(sockAddr)) < 0)
		{
			perror("Connect failed");
		}
	}

	Communicator::~Communicator()
	{
	}

	int Communicator::SendPacket(ThywinPacket packet)
	{
		std::stringstream data;
		data << packet.Method << SEP << packet.Type << SEP;
		data << packet.Content->Serialize() << EOT;

		const char* realdata = data.str().c_str();

		int sendSize = send(connectionSocket, realdata, strlen(realdata), 0);
		if (sendSize < 0)
		{
			perror("Send failed");
		}

		close(connectionSocket);

		return sendSize;
	}

	ThywinPacket* Communicator::ReceivePacket(TPObject* obj)
	{
		ThywinPacket* packet = new ThywinPacket;

		std::stringstream receiveBuffer;

		char c;
		int receiveSize;
		do
		{
			receiveSize = recv(connectionSocket, &c, 1, 0);

			receiveBuffer << c;
		} while (receiveSize > 0 && c != EOT);

		if (receiveSize < 0)
		{
			perror("Receive failed");
		}
		else if (receiveSize == 0)
		{
			//connection closed;
			close(connectionSocket);
			std::cout << "Master closed the connection" << std::endl;
			exit(EXIT_SUCCESS);
		}

		std::string token;
		std::getline(receiveBuffer, token, SEP);
		packet->Method = (PacketMethod) atoi(token.c_str());
		std::getline(receiveBuffer, token, SEP);
		packet->Type = (PacketType) atoi(token.c_str());

		//obj->Deserialize(receiveBuffer);
		std::cout << receiveBuffer;

		return packet;
	}

} /* namespace thywin */