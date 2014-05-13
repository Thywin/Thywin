/*
 * crawler.cpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas, Bobby
 */
#include <cstdio>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "crawler.h"

namespace thywin
{

	Crawler::Crawler(std::string ipaddress)
	{
		this->ipaddress = ipaddress;
	}

	void Crawler::CrawlUri()
	{
		struct sockaddr_in saddr;

		int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == -1)
		{
			perror("Socket failed");
		}

		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(7000);
		saddr.sin_addr.s_addr = inet_addr(ipaddress.c_str());

		if (connect(sock, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
		{
			perror("Creating connection failed");
		}

		ThywinPacket getUriMessage;
		getUriMessage.type = URI;
		getUriMessage.action = GET;

		if (send(sock, (void *) &getUriMessage, sizeof(ThywinPacket), NOFLAG) == -1)
		{
			perror("Send data failed");
		}

		std::string received;
		char buffer;
		while (recv(sock, &buffer, sizeof(buffer), NOFLAG) > 0)
		{
			received.push_back(buffer);
		}

		if (close(sock) == -1)
		{
			perror("Closing socket failed");
		}

		crawl(received);
	}

	int Crawler::crawl(std::string uri)
	{
		int pid;
		int status;
		int wgetCommunicationPipe[2];
		if (pipe(wgetCommunicationPipe) == -1)
		{
			perror("Piping failed");
			exit(EXIT_FAILURE);
		}

		switch (pid = fork())
		{
			case 0:
				startWget(wgetCommunicationPipe, uri);
				break;

			case -1:
				perror("Can\'t create child!");
				return -1;

			default:
				sendUriDocument(wgetCommunicationPipe, uri);
				pid = wait(&status);
		}
		return 0;
	}

	void Crawler::sendUriDocument(int* wgetCommunicationPipe, std::string uri)
	{
		if (close(wgetCommunicationPipe[1]) == -1)
		{
			perror("Closing document pipe failed");
		}

		char c;
		int readSize = read(wgetCommunicationPipe[0], &c, sizeof(c));

		struct sockaddr_in saddr;
		int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == -1)
		{
			perror("Socket failed");
		}

		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(7000);
		saddr.sin_addr.s_addr = inet_addr("192.168.100.11");

		if (connect(sock, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
		{
			perror("Connect failed");
		}

		ThywinPacket sendUriDocumentMessage;
		sendUriDocumentMessage.type = DOCUMENT;
		sendUriDocumentMessage.action = PUT;
		sendUriDocumentMessage.size = uri.size();

		if (send(sock, (void *) &sendUriDocumentMessage, sizeof(ThywinPacket), NOFLAG) == -1)
		{
			perror("Send failed");
		}

		std::cout << uri << std::endl;

		const char* charuri = uri.c_str();
		if (send(sock, charuri, strlen(charuri), NOFLAG) == -1)
		{
			perror("Send URI failed");
		}

		do
		{
			if (send(sock, &c, 1, NOFLAG) == -1)
			{
				perror("Send Document failed");
			}
		} while ((readSize = read(wgetCommunicationPipe[0], &c, sizeof(c))) > 0);

		if (close(wgetCommunicationPipe[0]) == -1)
		{
			perror("Closing document pipe failed");
		}

		if (close(sock) == -1)
		{
			perror("Closing socket failed");
		}
	}

	void Crawler::startWget(int* wgetCommunicationPipe, std::string uri)
	{
		if (close(wgetCommunicationPipe[0]) == -1)
		{
			perror("Closing document pipe failed");
		}

		if (close(STDOUT_FILENO) == -1)
		{
			perror("Closing stdout failed");
		}

		if (dup(wgetCommunicationPipe[1]) == -1)
		{
			perror("Closing document pipe failed");
		}

		char* exec[] =
		{ "wget", "-qO-", (char *) uri.c_str(), NULL };

		if (execvp(exec[0], exec) == -1)
		{
			perror("Execute failed");
			exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);
	}

}
