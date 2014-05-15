/*
 * Crawler.cpp
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

	void Crawler::CrawlURI()
	{
		struct sockaddr_in saddr;

		int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == -1)
		{
			perror("Socket creatation failed");
		}

		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(7000);
		saddr.sin_addr.s_addr = inet_addr(ipaddress.c_str());

		if (connect(sock, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
		{
			perror("Creating connection failed");
		}

		ThywinPacket getURIMessage;
		getURIMessage.type = URI;
		getURIMessage.action = GET;

		if (send(sock, (void *) &getURIMessage, sizeof(ThywinPacket), NOFLAG) == -1)
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

	int Crawler::crawl(std::string URI)
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
				startWget(wgetCommunicationPipe, URI);
				break;

			case -1:
				perror("Can\'t create child!");
				return -1;

			default:
				sendURIDocument(wgetCommunicationPipe, URI);
				pid = wait(&status);
		}
		return 0;
	}

	void Crawler::sendURIDocument(int* wgetCommunicationPipe, std::string URI)
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

		ThywinPacket sendURIDocumentMessage;
		sendURIDocumentMessage.type = DOCUMENT;
		sendURIDocumentMessage.action = PUT;
		sendURIDocumentMessage.size = URI.size();

		if (send(sock, (void *) &sendURIDocumentMessage, sizeof(ThywinPacket), NOFLAG) == -1)
		{
			perror("Send failed");
		}

		std::cout << URI << std::endl;

		const char* charURI = URI.c_str();
		if (send(sock, charURI, strlen(charURI), NOFLAG) == -1)
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

	void Crawler::startWget(int* wgetCommunicationPipe, std::string URI)
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

		// -qO- is ussed to download the file quiet and send it to STDOUT
		char* exec[] =
		{ "wget", "-qO-", (char *) URI.c_str(), NULL };

		if (execvp(exec[0], exec) == -1)
		{
			perror("Execute failed");
			exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);
	}

}
