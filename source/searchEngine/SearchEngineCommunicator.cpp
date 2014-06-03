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
#include <system_error>
#include "Logger.h"
#include "Communicator.h"
#include "DocumentVectorPacket.h"
#include "SearchEngine.h"

namespace thywin
{
	SearchEngineCommunicator::SearchEngineCommunicator(int clientCommunicationSocket) :
			logger("SearchEngine.log")
	{
		logger.Log(INFO, "Search Engine Communicator created.");
		this->clientCommunicationSocket = clientCommunicationSocket;
		connection = true;
	}
	
	SearchEngineCommunicator::~SearchEngineCommunicator()
	{
		CloseConnection();
	}
	
	void SearchEngineCommunicator::HandleConnection()
		{
			try
			{
					sendPacket(getSearchResults(receivePacket()));
			}
			catch (std::exception& e)
			{
				logger.Log(ERROR, "Catched an exception in a connection thread: " + std::string(e.what()));
				pthread_exit(NULL);
			}
			catch (...)
			{
				logger.Log(ERROR, "Unknown exception occurred in a connection thread");
				pthread_exit(NULL);
			}
		}
	
	void SearchEngineCommunicator::CloseConnection()
	{
		if (connection)
		{
			connection = false;
			
			if (close(clientCommunicationSocket) < 0)
			{
				logger.Log(WARNING, std::string("Can't close socket in thread: ") + std::string(strerror(errno)));
			}
		}
	}
	
	void SearchEngineCommunicator::sendPacket(std::string packet)
	{
		std::stringstream data;
		data << packet << TP_END_OF_PACKET;
		
		int sendSize = send(clientCommunicationSocket, (const char*) data.str().c_str(), data.str().size(), 0);
		if (sendSize < 0)
		{
			throw std::runtime_error(std::string(strerror(errno)));
		}
	}
	
	std::string SearchEngineCommunicator::receivePacket()
	{
		std::stringstream receivedMessage;
		char buffer;
		int receivedSize = -1;
		do
		{
			receivedSize = recv(clientCommunicationSocket, &buffer, sizeof(buffer), 0);
			if(receivedSize < 0)
			{
				throw std::system_error();
			}
			else if(receivedSize == 0)
			{
				logger.Log(INFO, "A client closed the connection");
			}
			else
			{
				receivedMessage << buffer;				
			}
		} while (receivedSize > 0 && buffer != TP_END_OF_PACKET);
		
		return receivedMessage.str();
	}
	
	std::string SearchEngineCommunicator::getSearchResults(std::string searchWords)
		{
			MultiURIPacket uriPackets = SearchEngine().Search(searchWords);
			return uriPackets.Serialize();
		}

} /* namespace thywin */
