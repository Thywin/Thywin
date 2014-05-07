/*
 * main.cpp
 *
 *  Created on: 23 apr. 2014
 *      Author: Thomas
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
//#include <pthread.h>
#include "master.hpp"


int main(int argc, char** argv)
{

	int port = 7000;
	thywin::master master;
	int serverDesc = master.setupServer(port);
	if (serverDesc < 0)
	{
		return 1;
	}

	struct sockaddr_in client;
	int c = sizeof(client);
	//pthread_t thread_id;
	while (1)
	{
		int clientDesc = accept(serverDesc, (struct sockaddr *) &client,
				(socklen_t*) &c);
		if (clientDesc < 0)
		{
			perror("Failed to accept");
		}
		else
		{
			printf("Connection accepted with client: %s port %i\n",
					inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			/*if (pthread_create(&thread_id, NULL, master.handleConnection,
					(void *) &clientDesc) < 0)
			{
				perror("could not create thread");
				return 1;
			}*/
			master.handleConnection(&clientDesc);
		}
	}

	return EXIT_SUCCESS;
}

void thywin::master::handleConnection(void *socket)
{
	int clientDesc = *(int *) socket;

	char clientMessage[2000];
	bzero(clientMessage, 2000);
	int received;
	while ((received = recv(clientDesc, clientMessage, 2000, 0)) > 0)
	{
		printf("Received bytes: %i; Send bytes: %i; Message: %s\n", received,
				strlen(clientMessage), clientMessage);
		send(clientDesc, clientMessage, strlen(clientMessage), 0);
		bzero(clientMessage, 2000);
	}

	printf("Closing connection\n");
	close(clientDesc);
	//return 0;
}

int thywin::master::setupServer(int port)
{
	struct sockaddr_in server;
	int serverDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverDesc < 0)
	{
		perror("Coudln't set up socket");
		return -1;
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	int on = 1;
	setsockopt(serverDesc, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (bind(serverDesc, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		perror("Couldn't bind socket");
		return -1;
	}
	if (listen(serverDesc, 128) < 0)
	{
		perror("Couldn't listen");
		return -1;
	}
	return serverDesc;
}
