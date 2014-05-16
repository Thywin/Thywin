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
	void *setUpConnectionWithClient(void *socket)
	{
		int client = *(int *) socket;
		ClientConnection connection = thywin::ClientConnection(client);
		connection.HandleConnection();
		printf("THREAD EXIT\n");
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
	void Server::SetUp(int port)
	{
		struct sockaddr_in server;
		int serverDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverDesc < 0)
		{
			perror("Coudln't set up socket");
		}
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(port);
		int on = 1;
		if (setsockopt(serverDesc, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		{
			perror("Failed to set up server socket");
			return;
		}
		if (bind(serverDesc, (struct sockaddr *) &server, sizeof(server)) < 0)
		{
			perror("Couldn't bind socket");
			return;
		}
		if (listen(serverDesc, 128) < 0)
		{
			perror("Couldn't listen");
			return;
		}
		serverSocket = serverDesc;
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
