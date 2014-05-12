/*
 * communicator.cpp
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

namespace thywin
{
	void Communicator::handleGetURI(int socket)
	{
		Master master;
		URIElement nextElement = master.GetNextURIElementFromQueue();
		char* reply = (char *) nextElement.URI.c_str();
		printf("Reply: %s\n", reply);
		if (send(socket, reply, strlen(reply), 0) < 0)
		{
			perror("Error with send");
		}
	}

	void Communicator::handleGetDocument(int socket)
	{
		thywin::Master master;
		documentElement returnElement = master.GetNextDocumentElementFromQueue();
		ThywinPacket container =
		{ 2, 2, returnElement.URI.size() };
		if (send(socket, (void *) &container, sizeof(ThywinPacket), 0) < 0)
		{
			perror("Error in get document request while sending Thywin packet");
			return;
		}
		printf("Sending document for URL: %s", returnElement.URI.c_str());
		if (send(socket, returnElement.URI.c_str(), returnElement.URI.size(), 0) < 0)
		{
			perror("Error in get document request while sending document URI");
			return;
		}
		if (!returnElement.content.empty())
		{
			if (send(socket, returnElement.content.c_str(), returnElement.content.size(), 0) < 0)
			{
				perror("Error in get document request while sending content");
				return;
			}
		}
		else
		{
			char c = '\0';
			send(socket, &c, 1, 0);
		}
	}

	void Communicator::handlePutURI(int socket, ThywinPacket container)
	{
		struct URIElement element;
		element.hostDocumentRelevance = 0;
		int sizeOfURL = (container.size);
		char url[sizeOfURL];
		if (recv(socket, &url, sizeof(url), 0) < 0)
		{
			perror("Error while receiving a URI");
		}
		element.URI = std::string(url);
		thywin::Master master;
		master.AddURIElementToQueue(element);
	}

	void Communicator::handlePutDocument(int socket, ThywinPacket container)
	{
		int sizeOfURL = (container.size);
		char url[sizeOfURL + 1];
		if (recv(socket, &url, sizeof(url), 0) < 0)
		{
			perror("Error occured while receiving URI in put Document");
			return;
		}
		url[sizeOfURL] = '\0';
		documentElement* docElement = new documentElement();
		char c;
		while ((recv(socket, &c, 1, 0)) > 0)
		{
			docElement->content.push_back(c);
		}
		docElement->URI = url;
		printf("Added new document to queue. Host URI: %s\n", docElement->URI.c_str());
		thywin::Master master;
		master.AddDocumentElementToQueue(docElement);
	}

	void Communicator::HandleConnection(int socket)
	{
		ThywinPacket requestPacket;
		int received = recv(socket, &requestPacket, sizeof(requestPacket), 0);
		if (received < 0)
		{
			perror("Error while receiving container");
			return;
		}
		printf("Received bytes: %i Action: %i | Type: %i\n", received, (requestPacket.action), (requestPacket.type));

		switch (requestPacket.action)
		{
			case ACTION_GET:
				if ((requestPacket.type) == TYPE_URI)
				{
					handleGetURI(socket);
				}
				else if ((requestPacket.type) == TYPE_DOCUMENT)
				{
					handleGetDocument(socket);
				}
				break;

			case ACTION_PUT:
				if ((requestPacket.type) == TYPE_URI)
				{
					handlePutURI(socket, requestPacket);
				}
				else if ((requestPacket.type) == TYPE_DOCUMENT)
				{
					handlePutDocument(socket, requestPacket);
				}
				break;
			default:
				break;
		}
		printf("Closed connection with client: ......\n");
		if (close(socket) < 0)
		{
			perror("error on close");
		}
	}

	int Communicator::SetupServer(int port)
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

}
