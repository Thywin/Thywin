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
#include <errno.h>
#include <stdexcept>
#include <pthread.h>

namespace thywin
{
	ClientConnection::ClientConnection(const int& client)
	{
		clientSocket = client;
		handlingConnection = false;
		connection = true;
	}
	
	ClientConnection::~ClientConnection()
	{
		try
		{
			CloseConnection();
		}
		catch (std::runtime_error& e)
		{
			printf("Connection already closed: %s\n", e.what());
		}
	}

	void ClientConnection::HandleGetRequest(ThywinPacket packet)
	{
		ThywinPacket returnPacket;
		returnPacket.Method = RESPONSE;
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

		try
		{
			SendPacket(returnPacket);
		}
		catch (std::runtime_error& e)
		{
			printf("Error while sending return packet: %s\n", e.what());
		}
	}

	void ClientConnection::HandlePutRequest(ThywinPacket packet)
	{
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
		if (hasConnection())
		{
			printf("Closing Connection\n");
			handlingConnection = false;
			connection = false;

			if (close(clientSocket) < 0)
			{
				throw std::runtime_error(std::string(strerror(errno)));
			}
			pthread_exit(NULL);
		}
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
					handleReceivedThywinPacket(returnPacket);
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
		}
		else
		{
			printf("Content is NULL\n");
		}
		data << TP_END_OF_PACKET;
		const char* realdata = data.str().c_str();
		printf("Sending packet of size: %i\n", data.str().size());
		int sendSize = send(clientSocket, realdata, data.str().size(), 0);
		if (sendSize < 0)
		{
			throw std::runtime_error(std::string(strerror(errno)));
		}
		printf("Sending completed\n");
		return sendSize;
	}

	ThywinPacket ClientConnection::ReceivePacket()
	{
		std::stringstream receiveBuffer;
		char characterReceiveBuffer;
		int receiveSize;
		do
		{
			receiveSize = recv(clientSocket, &characterReceiveBuffer, 1, 0);
			receiveBuffer << characterReceiveBuffer;
		} while (receiveSize > 0 && characterReceiveBuffer != TP_END_OF_PACKET);

		checkReceiveSize(receiveSize);
		ThywinPacket returnPacket = createThywinPacket(receiveBuffer);
		printf("Received packet: METHOD %i TYPE %i\n", returnPacket.Method, returnPacket.Type);
		return returnPacket;
	}

	void ClientConnection::fillThywinPacket(ThywinPacket& packet, std::stringstream& buffer)
	{
		std::string valueForPacket;
		std::getline(buffer, valueForPacket, TP_HEADER_SEPERATOR);
		packet.Method = (PacketMethod) atoi(valueForPacket.c_str());

		std::getline(buffer, valueForPacket, TP_HEADER_SEPERATOR);
		packet.Type = (PacketType) atoi(valueForPacket.c_str());

		if (packet.Method == PUT)
		{
			std::getline(buffer, valueForPacket, TP_HEADER_SEPERATOR);
			deserializePutObject(packet, valueForPacket);
		}
	}

	void ClientConnection::deserializePutObject(ThywinPacket& packet, std::string serializedObject)
	{
		if (packet.Method != PUT)
		{
			throw std::invalid_argument(std::string("Called deserialize object for a non PUT packet"));
		}
		switch (packet.Type)
		{
			case URI:
			{
				std::shared_ptr<URIPacket> uriPacket(new URIPacket);
				uriPacket->Deserialize(serializedObject);
				packet.Content = uriPacket;
				break;
			}
			case DOCUMENT:
			{
				std::shared_ptr<DocumentPacket> docPacket(new DocumentPacket);
				docPacket->Deserialize(serializedObject);
				packet.Content = docPacket;
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
			default:
			{

			}
		}
	}

	void ClientConnection::handleReceivedThywinPacket(const ThywinPacket& packet)
	{
		switch (packet.Method)
		{
			case GET:
				HandleGetRequest(packet);
				break;
			case PUT:
				HandlePutRequest(packet);
				break;
			case RESPONSE:
				break;
		}
	}

	ThywinPacket ClientConnection::createThywinPacket(std::stringstream& receiveBuffer)
	{
		ThywinPacket returnPacket;
		returnPacket.Method = GET;
		returnPacket.Type = URI;
		returnPacket.Content = NULL;
		fillThywinPacket(returnPacket, receiveBuffer);
		return returnPacket;
	}

	void ClientConnection::checkReceiveSize(const int& receiveSize)
	{
		if (receiveSize < 0)
		{
			std::string errorMessage(strerror(errno));
			try
			{
				CloseConnection();
			}
			catch (std::runtime_error& e)
			{
				printf("Error while attempting to close connection: %s\n", e.what());
			}
			throw std::runtime_error(errorMessage);
		}
		else if (receiveSize == 0)
		{
			try
			{
				CloseConnection();
				std::cout << "client closed the connection" << std::endl;
			}
			catch (std::runtime_error& e)
			{
				printf("Error while attempting to close connection: %s\n", e.what());
			}
		} // There are no other cases that would need to be handled
	}

} /* namespace thywin */
