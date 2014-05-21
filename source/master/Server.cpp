/*
 * Server.cpp
 *
 *  Created on: 13 mei 2014
 *      Author: Thomas Kooi
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "Server.h"
#include <string>
#include "ClientConnection.h"
#include <stdexcept>

namespace thywin
{
	/**
	 * For usage within pthread.
	 */
	void* setUpConnectionWithClient(void *socket)
	{
		try
		{
			int client = *(int *) socket;
			ClientConnection connection = thywin::ClientConnection(client);
			connection.HandleConnection(); /* Blocking call. Will wait until client closed connection */
			printf("Connection with client & Thread has been closed\n");
		}
		catch (std::exception& e)
		{
			printf("connection closed: %s\n",e.what());
		}
		pthread_exit(NULL);
		return (void *) 0;
	}

	Server::Server()
	{
		AMOUNT_OF_CONNECTIONS_ACCEPT = 127;
		connection = false;
		SetUp(7500); /* Needs a define in the library for the default server port */
		Listen();
	}
	Server::Server(const int port)
	{
		AMOUNT_OF_CONNECTIONS_ACCEPT = 127;
		connection = false;
		SetUp(port);
		Listen();
	}
	
	Server::~Server()
	{
		connection = false;
		close(serverSocket);
	}

	bool Server::HasConnection()
	{
		return connection;
	}

	void Server::SetUp(const int port)
	{
		int serverDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverDesc < 0)
		{
			std::string errorMessage(strerror(errno));
			throw std::runtime_error("Failed to create socket: " + errorMessage);
		}
		sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(port);

		const int on = 1; // what is this?
		if (setsockopt(serverDesc, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		{
			std::string errorMessage(strerror(errno));
			throw std::runtime_error("Failed to set up server socket: " + errorMessage);
		}

		if (bind(serverDesc, (struct sockaddr *) &server, sizeof(server)) < 0)
		{
			std::string errorMessage(strerror(errno));
			throw std::runtime_error("Failed bind socket to port: " + errorMessage);
		}
		connection = true;
		serverSocket = serverDesc;
	}

	void Server::Listen()
	{
		if (!HasConnection())
		{
			throw std::runtime_error("Couldn't listen because server has no connection");
		}

		if (listen(serverSocket, AMOUNT_OF_CONNECTIONS_ACCEPT) < 0)
		{
			std::string errorMessage(strerror(errno));
			throw std::runtime_error("Failed have socket listen: " + errorMessage);
		}

		struct sockaddr_in clientAddr;
		int sizeOfClientAddr = sizeof(clientAddr);
		pthread_t thread_id;

		while (HasConnection())
		{
			int client = accept(serverSocket, (struct sockaddr *) &clientAddr, (socklen_t*) &sizeOfClientAddr);
			printf("Connection accepted with client: %s port %i\n", inet_ntoa(clientAddr.sin_addr),
					ntohs(clientAddr.sin_port));
			if (pthread_create(&thread_id, NULL, setUpConnectionWithClient, (void *) &client) < 0)
			{
				perror("could not create thread");
			}
			else
			{
				pthread_detach(thread_id);
			}
		}
	}

}
