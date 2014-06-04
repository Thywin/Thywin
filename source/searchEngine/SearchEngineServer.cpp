/*
 * SearchEngineServer.cpp
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin Janssen
 *      Author: Thomas Kooi
 */

#include "SearchEngineServer.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>

#include "SearchEngineCommunicator.h"

namespace thywin
{
	SearchEngineServer::SearchEngineServer(const int port, const int maxNumberOfConnections) :
			logger("SearchEngine.log")
	{
		logger.Log(INFO, "Starting Search Engine Server.");
		this->maxNumberOfConnections = maxNumberOfConnections;
		socketBound = false;
		
		SetUp(port);
		Listen();
	}
	SearchEngineServer::~SearchEngineServer()
	{
		logger.Log(INFO, "Search Engine Server shutting down.");
		socketBound = false;
		close(listenSocketFD);
	}
	
	void SearchEngineServer::SetUp(const int port)
	{
		listenSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listenSocketFD < 0)
		{
			throw std::system_error();
		}
		
		const int optionalValue = 1;
		if (setsockopt(listenSocketFD, SOL_SOCKET, SO_REUSEADDR, &optionalValue, sizeof(int)) < 0)
		{
			throw std::system_error();
		}
		
		sockaddr_in socketAddress;
		socketAddress.sin_family = AF_INET;
		socketAddress.sin_addr.s_addr = INADDR_ANY;
		socketAddress.sin_port = htons(port);
		if (bind(listenSocketFD, (struct sockaddr *) &socketAddress, sizeof(struct sockaddr)) < 0)
		{
			throw std::system_error();
		}
		
		socketBound = true;
	}
	
	void SearchEngineServer::Listen()
	{
		if (socketBound)
		{
			if (listen(listenSocketFD, maxNumberOfConnections) < 0)
			{
				throw std::system_error();
			}
			
			struct sockaddr_in clientSocketAddress;
			socklen_t sizeOfClientSocketAddress = sizeof(clientSocketAddress);
			
			pthread_t threadID;
			
			while (socketBound)
			{
				int clientCommunicationSocket = accept(listenSocketFD, (struct sockaddr *) &clientSocketAddress,
						(socklen_t*) &sizeOfClientSocketAddress);
				
				std::stringstream logMessageStream;
				logMessageStream << "Connection accepted with client: " << inet_ntoa(clientSocketAddress.sin_addr)
						<< " port: " << ntohs(clientSocketAddress.sin_port);
				logger.Log(INFO, logMessageStream.str());
				
				int* threadArgument = (int*) malloc(sizeof(*threadArgument));
				*threadArgument = clientCommunicationSocket;
				if (pthread_create(&threadID, NULL, SearchEngineServer::setUpConnectionWithClient, threadArgument) < 0)
				{
					close(clientCommunicationSocket);
					throw std::system_error();
				}
				else
				{
					pthread_detach(threadID);
				}
			}
		}
		else
		{
			throw std::logic_error("SearchEngineServer couldn't listen because server has no socket bound");
		}
	}
	
	void* SearchEngineServer::setUpConnectionWithClient(void *argumentClientCommunicationSocket)
	{
		int clientCommunicationSocket = *((int *) argumentClientCommunicationSocket);
		free(argumentClientCommunicationSocket);
		
		SearchEngineCommunicator searchEngineCommunicator(clientCommunicationSocket);
		searchEngineCommunicator.HandleConnection(); /* Blocking call. Will wait until client closed connection */
		
		pthread_exit(NULL);
		return (void *) EXIT_SUCCESS;
	}

} /* namespace thywin */
