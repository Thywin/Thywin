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

namespace thywin
{
	/**
	 * For usage within pthread.
	 */
	void* setUpConnectionWithClient(void *socket)
	{
		int client = *(int *) socket;
		ClientConnection connection = thywin::ClientConnection(client);
		connection.HandleConnection();
		printf("Connection with client & Thread has been closed\n");
		pthread_exit(NULL);

		return (void *) 0;
	}

	Server::Server()
	{
		serverSocket = -1;
	}
	
	Server::~Server()
	{
		if (serverSocket != -1)
		{
			close(serverSocket);
		}
	}
	int Server::SetUp(const int& port)
	{
		if (serverSocket != -1)
		{
			close(serverSocket);
		}

		int serverDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverDesc < 0)
		{
			perror("Coudln't set up socket");
			return -1;
		}
		sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(port);
		const int on = 1; // what is this?

		if (setsockopt(serverDesc, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		{
			perror("Failed to set up server socket");
			return -2;
		}

		if (bind(serverDesc, (struct sockaddr *) &server, sizeof(server)) < 0)
		{
			perror("Couldn't bind socket");
			return -3;
		}

		if (listen(serverDesc, 128) < 0)
		{
			perror("Couldn't listen");
			return -4;
		}
		serverSocket = serverDesc;
		return serverDesc;
	}

	void Server::Listen()
	{
		if (serverSocket < 0)
		{
			printf("Server was not yet initialized!\n");
			return;
		}

		struct sockaddr_in clientAddr;
		int c = sizeof(clientAddr);

		pthread_t thread_id;
		while (1)
		{
			int client = accept(serverSocket, (struct sockaddr *) &clientAddr, (socklen_t*) &c);
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
