/*
 * ParserCommunicator.cpp
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#include "ParserCommunicator.h"
#include <string.h>

namespace thywin
{

	ParserCommunicator::ParserCommunicator(std::string documentQueue, std::string URLQueue, std::string indexStore)
	{
		masterSockAddrs[eDocumentQueue].sin_addr.s_addr = inet_addr(documentQueue.c_str());
		masterSockAddrs[eURLQueue].sin_addr.s_addr = inet_addr(URLQueue.c_str());
		masterSockAddrs[eIndexStore].sin_addr.s_addr = inet_addr(indexStore.c_str());

		for (int i = 0; i < 3; i++)
		{
			masterSockAddrs[i].sin_family = AF_INET;
			masterSockAddrs[i].sin_port = htons(7000);
		}
	}

	ParserCommunicator::~ParserCommunicator()
	{
	}

	Document ParserCommunicator::GetDocumentFromQueue()
	{
		Document doc;
		masterSockets[eDocumentQueue] = socket(AF_INET, SOCK_STREAM, 0);
		if (masterSockets[eDocumentQueue] == -1)
		{
			perror("Socket failed");
			exit(EXIT_FAILURE);
		}

		if (connect(masterSockets[eDocumentQueue], (struct sockaddr*) &masterSockAddrs[eDocumentQueue],
				sizeof(masterSockAddrs[eDocumentQueue])) < 0)
		{
			perror("Connect failed");
		}

		RequestPacket requestPacket;
		requestPacket.type = DOCUMENT;
		requestPacket.action = GET;

		if (send(masterSockets[eDocumentQueue], &requestPacket, sizeof(RequestPacket), 0) < 0)
		{
			perror("Send failed");
		}

		RequestPacket responsePacket;
		if (recv(masterSockets[eDocumentQueue], &responsePacket, sizeof(RequestPacket), 0) < 0)
		{
			perror("Receive failed");
		}

		char url[responsePacket.size + 1];
		if (recv(masterSockets[eDocumentQueue], (void*) url, responsePacket.size, 0) < 0)
		{
			perror("Receive failed");
		}
		url[responsePacket.size] = '\0';
		printf("URL TERUGKRIJG : %s size:%d  stringsize:%d \n", url, responsePacket.size, strlen(url));
		doc.URI = std::string(url);

		char c;
		int receiveSize = recv(masterSockets[eDocumentQueue], &c, 1, 0);
		while (receiveSize > 0)
		{
			doc.content.push_back(c);
			receiveSize = recv(masterSockets[eDocumentQueue], &c, 1, 0);
		}

		if (receiveSize < 0)
		{
			perror("Receive failed");
		}
		close(masterSockets[eDocumentQueue]);

		return doc;
	}

	void ParserCommunicator::StoreIndex(DocumentVector index)
	{

	}

	void ParserCommunicator::StoreExpectedURIRelevance(URIRelevance uriRelevance)
	{
		masterSockets[eURLQueue] = socket(AF_INET, SOCK_STREAM, 0);
		if (masterSockets[eURLQueue] == -1)
		{
			perror("Socket failed");
			exit(EXIT_FAILURE);
		}

		if (connect(masterSockets[eURLQueue], (struct sockaddr*) &masterSockAddrs[eURLQueue],
				sizeof(masterSockAddrs[eURLQueue])) < 0)
		{
			perror("Connect failed");
		}

		RequestPacket requestPacket;
		requestPacket.type = URL;
		requestPacket.action = PUT;
		requestPacket.size = uriRelevance.URI.size();

		if (send(masterSockets[eURLQueue], &requestPacket, sizeof(RequestPacket), 0) < 0)
		{
			perror("Send failed");
		}

		if (send(masterSockets[eURLQueue], (char*) (uriRelevance.URI.c_str()), strlen(uriRelevance.URI.c_str()), 0) < 0)
		{
			perror("Send failed");
		}

		close(masterSockets[eURLQueue]);
	}

	void ParserCommunicator::StoreActualURIRelevance(URIRelevance uriRelevance)
	{

	}

} /* namespace thywin */
