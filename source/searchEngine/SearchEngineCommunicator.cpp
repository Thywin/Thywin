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
	SearchEngineCommunicator::SearchEngineCommunicator(int clientCommunicationSocket) : logger("SearchEngine.log")
	{
		logger.Log(INFO, "Search Engine Communicator created.");
		this->clientCommunicationSocket = clientCommunicationSocket;
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
			
			if (close(clientCommunicationSocket) < 0)
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
		
		int sendSize = send(clientCommunicationSocket, (const char*) data.str().c_str(), data.str().size(), 0);
		if (sendSize < 0)
		{
			throw std::runtime_error(std::string(strerror(errno)));
		}
		return sendSize;
	}
	
	std::string SearchEngineCommunicator::receivePacket()
	{
		std::stringstream receivedMessage;
		char buffer;
		int receivedSize = -1;
		do
		{
			receivedSize = recv(clientCommunicationSocket, &buffer, sizeof(buffer), 0);
			checkReceiveSize(receivedSize);
			receivedMessage << buffer;
		} while (receivedSize > 0 && buffer != TP_END_OF_PACKET);
		return receivedMessage.str();
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
