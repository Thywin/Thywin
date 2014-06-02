/*
 * SearchEngineCommunicator.cpp
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin
 */

#include "SearchEngineCommunicator.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <string.h>
#include <string>
#include <errno.h>
#include <stdexcept>
#include "Logger.h"
#include "Communicator.h"
#include "DocumentVectorPacket.h"
#include "SearchEngine.h"

namespace thywin
{
	
	/**
	 * Loggers are temporarily placed in comments while awaiting library update
	 */
	SearchEngineCommunicator::SearchEngineCommunicator(int client)
	{
		std::stringstream out;
		out << "Master_connection_" << client << ".log";
		std::string logFileName = out.str();
		clientSocket = client;
		handlingConnection = false;
		connection = true;
	}
	
	SearchEngineCommunicator::~SearchEngineCommunicator()
	{
		handlingConnection = false;
		connection = false;
		CloseConnection();
	}
	
	std::string SearchEngineCommunicator::getSearchResults(std::string searchWords)
	{
		SearchEngine searchEngine;
		MultiURIPacket uriPackets = searchEngine.Search(searchWords);
		return uriPackets.Serialize();
	}
	
	bool SearchEngineCommunicator::hasConnection()
	{
		return connection;
	}
	
	void SearchEngineCommunicator::CloseConnection()
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
	
	void SearchEngineCommunicator::HandleConnection()
	{
		while (true) {
			sendPacket(getSearchResults(receivePacket()));
		}
	}
	
	int SearchEngineCommunicator::sendPacket(std::string packet)
	{
		std::stringstream data;
		data << packet << TP_END_OF_PACKET;
		
		int sendSize = send(clientSocket, (const char*) data.str().c_str(), data.str().size(), 0);
		if (sendSize < 0)
		{
			throw std::runtime_error(std::string(strerror(errno)));
		}
		return sendSize;
	}
	
	std::string SearchEngineCommunicator::receivePacket()
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
		return receiveBuffer.str();
	}
		
	void SearchEngineCommunicator::checkReceiveSize(const int receiveSize)
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
