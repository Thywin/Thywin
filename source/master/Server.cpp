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
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>
#include "Server.h"
#include "ClientConnection.h"
#include "Logger.h"

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
			close(client);
			printf("connection closed: %s\n", e.what());
		}
		pthread_exit(NULL);
		return (void *) 0;
	}

	Logger Server::logger("Master.log");

	Server::Server()
	{
		logger.log(INFO,std::string("Server starting on default port and default amount of connections accept"));
		amountOfConnectionsAccept = AMOUNT_OF_CONNECTIONS_ACCEPT_DEFAULT;
		connection = false;
		SetUp(7500); /* Needs a define in the library for the default server port */
		Listen();
	}
	Server::Server(const int port)
	{
		logger.log(INFO,std::string("Server starting on port # and default amount of connections accept"));
		amountOfConnectionsAccept = AMOUNT_OF_CONNECTIONS_ACCEPT_DEFAULT;
		connection = false;
		SetUp(port);
		Listen();
	}

	Server::Server(const int port, const int accept)
	{
		logger.log(INFO,std::string("Server starting on port # and amount of connections accept #"));
		amountOfConnectionsAccept = accept;
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
		logger.log(INFO,std::string("Socket has been created"));

		connection = true;
		serverSocket = serverDesc;
	}

	void Server::Listen()
	{
		if (!HasConnection())
		{
			throw std::runtime_error("Couldn't listen because server has no connection");
		}
		if (listen(serverSocket, amountOfConnectionsAccept) < 0)
		{
			std::string errorMessage(strerror(errno));
			throw std::runtime_error("Failed have socket listen: " + errorMessage);
		}

		struct sockaddr_in clientAddr;
		int sizeOfClientAddr = sizeof(clientAddr);
		pthread_t thread_id;

		logger.log(INFO,std::string("Server is awaiting connections"));

		while (HasConnection())
		{
			int client = accept(serverSocket, (struct sockaddr *) &clientAddr, (socklen_t*) &sizeOfClientAddr);
			printf("Connection accepted with client: %s port %i\n", inet_ntoa(clientAddr.sin_addr),
					ntohs(clientAddr.sin_port));

			logger.log(INFO,std::string("Connection accepted with client: %s port %i"));
			if (pthread_create(&thread_id, NULL, setUpConnectionWithClient, (void *) &client) < 0)
			{
				close(client);
				logger.log(ERROR,std::string("Failed to create a new thread for client connection"));
				throw std::string("Failed to create a new thread for client connection");
			}
			else
			{
				pthread_detach(thread_id);
			}
		}
	}

}
