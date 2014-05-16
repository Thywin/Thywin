/*
 * ClientConnection.cpp
 *
 *  Created on: 14 mei 2014
 *      Author: Thomas Kooi
 */

#include "ClientConnection.h"
#include "Communicator.h"
#include "MasterCommunicator.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <memory>
#include <string.h>

namespace thywin
{
	ClientConnection::ClientConnection(int client)
	{
		clientSocket = client;
		handlingConnection = false;
		connection = true;
	}
	
	ClientConnection::~ClientConnection()
	{
		if (connection)
		{
			handlingConnection = false;
			close(clientSocket);
		}
	}

	void ClientConnection::HandleGetRequests(ThywinPacket packet)
	{
		printf("Handling a GET request\n");
		ThywinPacket returnPacket;
		returnPacket.Method = RESPONSE;
		MasterCommunicator communicator;
		switch (packet.Type)
		{
			case URI:
				returnPacket = communicator.HandleGetURI();
				break;
			case DOCUMENT:
				returnPacket = communicator.HandleGetDocument();
				break;
			case RELEVANCE:
				//returnPacket = MasterCommunicator::HandleGetRelevance(); // to be implemented later
				break;
			case DOCUMENTVECTOR:
				//returnPacket = MasterCommunicator::HandleGetDocumentVector(); // to be implemented later
				break;
		}
		SendPacket(returnPacket);
	}

	void ClientConnection::HandlePutRequests(ThywinPacket packet)
	{
		printf("Handling a PUT request\n");
		MasterCommunicator communicator;

		switch (packet.Type)
		{
			case URI:
				communicator.HandlePutURI(packet.Content);
				break;
			case DOCUMENT:
				communicator.HandlePutDocument(packet.Content);
				break;
			case RELEVANCE:
				//MasterCommunicator::HandlePutRelevance(packet);	// to be implemented later
				break;
			case DOCUMENTVECTOR:
				//MasterCommunicator::HandlePutDocumentVector(packet);	// to be implemented later
				break;
		}
	}

	bool ClientConnection::hasConnection()
	{
		return connection;
	}

	void ClientConnection::CloseConnection()
	{
		printf("Closing Connection\n");
		handlingConnection = false;
		close(clientSocket);
		connection = false;
	}

	void ClientConnection::HandleConnection()
	{
		if (!handlingConnection)
		{
			handlingConnection = true;
			while (handlingConnection)
			{
				printf("waiting for new packet\n");
				ThywinPacket returnPacket = ReceivePacket();
				if (connection)
				{
					switch (returnPacket.Method)
					{
						case GET:
							HandleGetRequests(returnPacket);
							break;
						case PUT:
							HandlePutRequests(returnPacket);
							break;
						case RESPONSE:
							break;
					}
				}
			}
		}
	}
	int ClientConnection::SendPacket(ThywinPacket packet)
	{

		std::stringstream data;
		data << packet.Method << TP_HEADER_SEPERATOR << packet.Type << TP_HEADER_SEPERATOR;
		if (packet.Content != NULL)
		{
			data << packet.Content->Serialize();
		} else {
			printf("CONTENT FOUND WAS NULL\n");
		}
		data << TP_END_OF_PACKET;
		const char* realdata = data.str().c_str();
		printf("Sending packet of size: %i\n", data.str().size());
		int sendSize = send(clientSocket, realdata, data.str().size(), 0);
		if (sendSize < 0)
		{
			perror("Send failed");
		}
		printf("Sending completed\n");
		return sendSize;
	}
	ThywinPacket ClientConnection::ReceivePacket()
	{
		ThywinPacket returnPacket;
		std::stringstream receiveBuffer;
		const char TP_HEADER_SEPERATOR = (char) 30; // cannot find it in .h files??
		const char TP_END_OF_PACKET = (char) 4;
		char c;
		int receiveSize;
		do
		{
			receiveSize = recv(clientSocket, &c, 1, 0);
			receiveBuffer << c;
		} while (receiveSize > 0 && c != TP_END_OF_PACKET);
		returnPacket.Method = GET;
		returnPacket.Type = URI;
		returnPacket.Content = NULL;
		if (receiveSize < 0)
		{
			perror("Receive failed");
			CloseConnection();
			return returnPacket;
		}
		else if (receiveSize == 0)
		{
			CloseConnection();
			std::cout << "client closed the connection" << std::endl;
			return returnPacket;
		}
		std::string token;
		std::getline(receiveBuffer, token, TP_HEADER_SEPERATOR);
		returnPacket.Method = (PacketMethod) atoi(token.c_str());
		std::getline(receiveBuffer, token, TP_HEADER_SEPERATOR);
		returnPacket.Type = (PacketType) atoi(token.c_str());
		std::getline(receiveBuffer, token, TP_HEADER_SEPERATOR);

		if (returnPacket.Method == PUT)
		{
			switch (returnPacket.Type)
			{
				case URI:
				{
					std::shared_ptr<URIPacket> uriPacket(new URIPacket);
					uriPacket->Deserialize(token);
					returnPacket.Content = uriPacket;
					break;
				}
				case DOCUMENT:
				{
					std::shared_ptr<DocumentPacket> docPacket(new DocumentPacket);
					docPacket->Deserialize(token);
					returnPacket.Content = docPacket;
					break;
				}
				case RELEVANCE:
				{
					break;
				}
				case DOCUMENTVECTOR:
				{
					break;
				}
			}
		}

		printf("Received packet: METHOD %i TYPE %i\n", returnPacket.Method, returnPacket.Type);
		return returnPacket;
	}
} /* namespace thywin */
