/*
 * SearchEngineServer.cpp
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin
 */

#include "SearchEngineServer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>
#include "Logger.h"
#include "SearchEngineCommunicator.h"
namespace thywin
{
	
	void* setUpConnectionWithClient(void *socket)
	{
		try
		{
			int client = *((int *) socket);
			free(socket);
			std::cout << "Connected with client: " << client <<std::endl;
			SearchEngineCommunicator communicator(client);
			communicator.HandleConnection(); /* Blocking call. Will wait until client closed connection */
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "unexpected exception." << std::endl;
		}
		pthread_exit (NULL);
		return (void *) 0;
	}
	
	SearchEngineServer::SearchEngineServer(const int port, const int accept)
	{
		maximumNumbersOfConnections = accept;
		connection = false;
		SetUp(port);
		Listen();
	}
	
	SearchEngineServer::~SearchEngineServer()
	{
		connection = false;
		close(serverSocket);
	}
	
	bool SearchEngineServer::HasConnection()
	{
		return connection;
	}
	
	void SearchEngineServer::SetUp(const int port)
	{
		int serverDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverDescriptor < 0)
		{
			std::string errorMessage(strerror (errno));
			throw std::runtime_error("Failed to create socket: " + errorMessage);
		}
		sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(port);
		
		const int optionalValue = 1;
		if (setsockopt(serverDescriptor, SOL_SOCKET, SO_REUSEADDR, &optionalValue, sizeof(optionalValue)) < 0)
		{
			std::string errorMessage(strerror (errno));
			throw std::runtime_error("Failed to set up server socket: " + errorMessage);
		}
		
		if (bind(serverDescriptor, (struct sockaddr *) &server, sizeof(server)) < 0)
		{
			std::string errorMessage(strerror (errno));
			throw std::runtime_error("Failed bind socket to port: " + errorMessage);
		}
		
		connection = true;
		serverSocket = serverDescriptor;
	}
	
	void SearchEngineServer::Listen()
	{
		if (!HasConnection())
		{
			throw std::runtime_error("Couldn't listen because server has no connection");
		}
		if (listen(serverSocket, maximumNumbersOfConnections) < 0)
		{
			std::string errorMessage(strerror (errno));
			throw std::runtime_error("Failed have socket listen: " + errorMessage);
		}
		
		struct sockaddr_in clientAddr;
		socklen_t sizeOfClientAddr = sizeof(clientAddr);
		pthread_t thread_id;
		
		while (HasConnection())
		{
			int client = accept(serverSocket, (struct sockaddr *) &clientAddr, (socklen_t*) &sizeOfClientAddr);
			printf("Connection accepted with client: %s port %i | CLIENT: %i\n", inet_ntoa(clientAddr.sin_addr),
					ntohs(clientAddr.sin_port), client);
			
			
			int* arg = (int*) malloc(sizeof(*arg));
			*arg = client;
			if (pthread_create(&thread_id, NULL, setUpConnectionWithClient, arg) < 0)
			{
				close(client);

				throw std::runtime_error("Failed to create a new thread for client connection");
			}
			else
			{
				pthread_detach(thread_id);
			}
		}
	}

} /* namespace thywin */
