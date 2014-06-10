/*
 * ClientConnection.cpp
 *
 *  Created on: 14 mei 2014
 *      Author: Thomas Kooi
 *      Author: Thomas Gerritsen
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <string.h>
#include <string>
#include <errno.h>
#include <stdexcept>
#include <system_error>
#include "Logger.h"
#include "ClientConnection.h"
#include "Communicator.h"
#include "MasterCommunicator.h"
#include "MultiURIPacket.h"
#include "DocumentVectorPacket.h"

namespace thywin
{

	ClientConnection::ClientConnection(int client, Logger& threadLogger) :
			logger(threadLogger)
	{
		clientSocket = client;
		handlingConnection = false;
	}

	ClientConnection::~ClientConnection()
	{
		CloseConnection();
	}

	void ClientConnection::HandleGetRequest(const ThywinPacket& packet)
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
			default:
				break;
		}
		SendPacket(returnPacket);
	}

	void ClientConnection::HandlePutRequest(const ThywinPacket& packet)
	{
		switch (packet.Type)
		{
			case URI:
				communicator.HandlePutURI(packet.Content);
				break;
			case DOCUMENT:
				communicator.HandlePutDocument(packet.Content);
				break;
			case DOCUMENTVECTOR:
				communicator.HandlePutDocumentVector(packet.Content);
				break;
			case URIVECTOR:
				communicator.HandlePutUriVector(packet.Content);
				break;
			default:
				break;
		}
	}

	bool ClientConnection::hasConnection()
	{
		return handlingConnection;
	}

	void ClientConnection::CloseConnection()
	{
		if (handlingConnection)
		{
			handlingConnection = false;

		    close(clientSocket);
		    //If close fails it doesn't matter.
		}
	}

	void ClientConnection::HandleConnection()
	{
		handlingConnection = true;
		while (handlingConnection)
		{
			ThywinPacket returnPacket = ReceivePacket();
			handleReceivedThywinPacket(returnPacket);
		}
	}

	void ClientConnection::SendPacket(ThywinPacket& packet)
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
			int bytesSent = send(clientSocket, &sendBuffer[totalBytesSent], data.str().size() - totalBytesSent, 0);

			if (bytesSent < 0)
			{
				std::stringstream sendError;
				sendError << "Failed to send: " << strerror(errno);
				logger.Log(ERROR, sendError.str());
				throw std::system_error();
			}

			totalBytesSent += bytesSent;
		}
	}

	ThywinPacket ClientConnection::ReceivePacket()
	{
		std::stringstream receiveBuffer;
		char characterReceiveBuffer;
		int receiveSize = -1;
		do
		{
			receiveSize = recv(clientSocket, &characterReceiveBuffer, 1, 0);
			checkReceiveSize(receiveSize);
			receiveBuffer << characterReceiveBuffer;
		} while (receiveSize > 0 && characterReceiveBuffer != TP_END_OF_PACKET);
		return createThywinPacket(receiveBuffer);
	}

	void ClientConnection::fillThywinPacket(ThywinPacket& packet, std::stringstream& buffer)
	{
		std::string valueForPacket;
		std::getline(buffer, valueForPacket, TP_HEADER_SEPERATOR);
		packet.Method = (PacketMethod) std::stoi(valueForPacket);

		std::getline(buffer, valueForPacket, TP_HEADER_SEPERATOR);
		packet.Type = (PacketType) std::stoi(valueForPacket);

		if (packet.Method == PUT)
		{
			std::getline(buffer, valueForPacket, TP_HEADER_SEPERATOR);
			deserializePutObject(packet, valueForPacket);
		}
	}

	void ClientConnection::deserializePutObject(ThywinPacket& packet, std::string& serializedObject)
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
			case DOCUMENTVECTOR:
			{
				std::shared_ptr<DocumentVectorPacket> docPacket(new DocumentVectorPacket);
				docPacket->Deserialize(serializedObject);
				packet.Content = docPacket;
				break;
			}
			case URIVECTOR:
			{
				std::shared_ptr<MultiURIPacket> multiURIPacket(new MultiURIPacket);
				multiURIPacket->Deserialize(serializedObject);
				packet.Content = multiURIPacket;
				break;
			}
			default:
				break;
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
		ThywinPacket returnPacket(GET, URI);
		fillThywinPacket(returnPacket, receiveBuffer);
		return returnPacket;
	}

	void ClientConnection::checkReceiveSize(const int receiveSize)
	{
		if (receiveSize < 0)
		{
			std::string errorMessage(strerror(errno));
			CloseConnection();
			throw std::runtime_error(errorMessage);
		}
		else if (receiveSize == 0)
		{
			logger.Log(INFO, "Client Closed Connection\n");
			CloseConnection();
			throw std::runtime_error("Client Closed Connection");
		} // There are no other cases that would need to be handled
	}

} /* namespace thywin */
