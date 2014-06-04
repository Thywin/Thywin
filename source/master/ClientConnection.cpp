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

	/**
	 * Loggers are temporarily placed in comments while awaiting library update
	 */
	ClientConnection::ClientConnection(int client)
	{
		std::stringstream out;
		out << "Master_connection_" << client << ".log";
		std::string logFileName = out.str();
		clientSocket = client;
		handlingConnection = false;
		connection = true;
	}

	ClientConnection::~ClientConnection()
	{
		handlingConnection = false;
		connection = false;
		CloseConnection();
	}

	void ClientConnection::HandleGetRequest(const ThywinPacket& packet)
	{
		ThywinPacket returnPacket;
		returnPacket.Method = RESPONSE;
		try
		{
			SendPacket(returnPacket);
		}
		catch (std::runtime_error& e)
		{
			printf("Error while sending return packet: %s\n", e.what());
		}
	}

	void ClientConnection::HandlePutRequest(const ThywinPacket& packet)
	{
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
				// who cares?
			}
		}
	}

	void ClientConnection::HandleConnection()
	{
		handlingConnection = true;
		while (handlingConnection)
		{
			ThywinPacket returnPacket = ReceivePacket();
			if (hasConnection())
			{
				handleReceivedThywinPacket(returnPacket);
			}
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
		try
		{
			fillThywinPacket(returnPacket, receiveBuffer);
		}
		catch (std::exception& e)
		{
			printf("Couldn't fill ThywinPacket\n");
		}
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
			printf("Client Closed Connection\n");
			CloseConnection();
			throw std::runtime_error("Client Closed Connection");
		} // There are no other cases that would need to be handled
	}

} /* namespace thywin */
