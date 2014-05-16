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
#include <algorithm>
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

		int socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (socketFD == -1)
		{
			perror("Socket creatation failed");
		}

		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(7000);
		saddr.sin_addr.s_addr = inet_addr(ipaddress.c_str());

		if (connect(socketFD, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
		{
			perror("Creating connection failed");
			return;
		}

		ThywinPacket getURIMessage;
		getURIMessage.type = URI;
		getURIMessage.action = GET;

		if (send(socketFD, (void *) &getURIMessage, sizeof(ThywinPacket), NOFLAG) == -1)
		{
			perror("Send data failed");
			return;
		}

		std::string received;
		char buffer;
		while (recv(socketFD, &buffer, sizeof(buffer), NOFLAG) > 0)
		{
			received.push_back(buffer);
		}

		if (close(socketFD) == -1)
		{
			perror("Closing socket failed");
		}

		crawl(received);
	}

	int Crawler::crawl(const std::string& URI)
	{
		int pagePipe[2];
		if (pipe(pagePipe) == -1)
		{
			perror("Piping failed");
			exit(EXIT_FAILURE);
		}

		int processID = -1;
		int status = 0;
		switch (processID = fork())
		{
			case 0:
				startCurl(pagePipe, URI);
				break;

			case -1:
				perror("Can\'t create child!");
				return -1; // Exception van maken

			default:
				processID = wait(&status);
				sendURIDocument(pagePipe, URI);

		}
		return 0;
	}

	void Crawler::sendURIDocument(int* pagePipe, const std::string& URI)
	{
		if (close(pagePipe[1]) == -1)
		{
			perror("Closing document pipe failed");
		}

		char buffer;
		int readSize = read(pagePipe[0], &buffer, sizeof(buffer));

		std::string document = "";
		do
		{
			document.push_back(buffer);
		} while ((readSize = read(pagePipe[0], &buffer, sizeof(buffer))) > 0);

		std::string::size_type headerend = document.find("\r\n\r\n");
		std::string head = document.substr(0, headerend);
		std::transform(head.begin(), head.end(), head.begin(), ::tolower);
		std::string body = "";

		std::string::size_type content_type_html = head.find("content-type: text/html");
		if (content_type_html != std::string::npos)
		{
			body = document.substr(headerend, document.size());
			std::cout << "Valid link: " << URI << std::endl;
		}
		else
		{
			std::cout << "Invalid link: " << URI << std::endl;
			return;
		}

		struct sockaddr_in saddr;
		int socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (socketFD == -1)
		{
			perror("Socket failed");
		}

		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(7000);
		saddr.sin_addr.s_addr = inet_addr(ipaddress.c_str());

		if (connect(socketFD, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
		{
			perror("Connect failed");
		}

		ThywinPacket sendURIDocumentMessage;
		sendURIDocumentMessage.type = DOCUMENT;
		sendURIDocumentMessage.action = PUT;
		sendURIDocumentMessage.size = URI.size();

		if (send(socketFD, (void *) &sendURIDocumentMessage, sizeof(ThywinPacket), NOFLAG) == -1)
		{
			perror("Send failed");
		}

		std::cout << URI << std::endl;

		const char* charURI = URI.c_str();
		if (send(socketFD, charURI, strlen(charURI), NOFLAG) == -1)
		{
			perror("Send URI failed");
		}

		if (send(socketFD, body.c_str(), body.size(), NOFLAG) == -1)
		{
			perror("Sending document failed");
		}

		if (close(pagePipe[0]) == -1)
		{
			perror("Closing document pipe failed");
		}

		if (close(socketFD) == -1)
		{
			perror("Closing socket failed");
		}
	}

	void Crawler::startCurl(int* pageBodyPipe, const std::string& URI)
	{

		if (close(pageBodyPipe[0]) == -1)
		{
			perror("Closing head pipe failed");
		}

		if (dup2(pageBodyPipe[1], STDOUT_FILENO) == -1)
		{
			perror("Error dupping stderr");
			exit(EXIT_FAILURE);
		}

		if (close(STDERR_FILENO) == -1)
		{
			perror("Closing stderr failed");
		}

		char* exec[] =
		{ "curl", "-i", (char *) URI.c_str(), NULL };

		if (execvp(exec[0], exec) == -1)
		{
			perror("Execute failed");
			exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);
	}

}
