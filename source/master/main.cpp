/*
 * main.cpp
 *
 *  Created on: 10 mei 2014
 *      Author: Thomas
 */
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <mutex>
#include <fstream>
#include "master.hpp"
#include "communicator.hpp"
#include <semaphore.h>

void *awaitClient(void *socket);

int main()
{
	int port = 7000;
	thywin::Communicator communicator;
	int serverDesc = communicator.SetupServer(port);
	if (serverDesc < 0)
	{
		return 1;
	}
	pthread_t thread_id;
	for (int i = 0; i < 10; i++)
	{
		if (pthread_create(&thread_id, NULL, awaitClient, (void *) &serverDesc) < 0)
		{
			perror("could not create thread");
			return 1;
		}
	}

	while (1)
	{
	}
	printf("System shutdown\n");
	return EXIT_SUCCESS;
}

void *awaitClient(void *socket)
{
	struct sockaddr_in client;
	int serverDesc = *(int *) socket;
	int c = sizeof(client);
	thywin::Communicator communicator;
	while (1)
	{
		int clientDesc = accept(serverDesc, (struct sockaddr *) &client, (socklen_t*) &c);
		printf("Connection accepted with client: %s port %i\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		communicator.HandleConnection(clientDesc);
	}
	return (void *) 0;
}
