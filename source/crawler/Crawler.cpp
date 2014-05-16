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
#include "Communicator.h"
#include "ThywinPacket.h"
#include "URIPacket.h"
#include "DocumentPacket.h"
#include <Memory>

namespace thywin
{

	Crawler::Crawler(const std::string& ipaddress, const int& port) :
			communication(ipaddress, port)
	{
	}

	void Crawler::CrawlURI()
	{
		ThywinPacket packet;
		packet.Type = URI;
		packet.Method = GET;
		packet.Content = NULL;
		communication.SendPacket(packet);
		std::shared_ptr<URIPacket> uriPacket(new URIPacket);
		std::shared_ptr<ThywinPacket> receivedPacket(new ThywinPacket);
		receivedPacket = communication.ReceivePacket(uriPacket);

		if (receivedPacket->Type == URI && receivedPacket->Method == RESPONSE)
		{
			crawl(uriPacket->URI);
		}
	}

	int Crawler::crawl(const std::string& URI)
	{
		int pagePipe[2];
		if (pipe(pagePipe) == -1)
		{
			perror("Piping failed");
			exit(EXIT_FAILURE);
		}

		int processID = fork();
		switch (processID)
		{
			case 0:
				startCurl(pagePipe, URI);
				break;

			case -1:
				perror("Can\'t create child!");
				return -1; // Exception van maken

			default:
				processID = wait(NULL);
				sendURIDocument(pagePipe, URI);

		}
		return 0;
	}

	void Crawler::sendURIDocument(int* pagePipe, const std::string& CrawledURI)
	{
		if (close(pagePipe[1]) == -1)
		{
			perror("Closing document pipe failed");
		}

		char buffer;
		int readSize = read(pagePipe[0], &buffer, sizeof(buffer));

		std::string document;
		do
		{
			document.push_back(buffer);
		} while ((readSize = read(pagePipe[0], &buffer, sizeof(buffer))) > 0);

		std::string::size_type headerend = document.find("\r\n\r\n");
		std::string head = document.substr(0, headerend);
		std::transform(head.begin(), head.end(), head.begin(), ::tolower);
		std::string body = "";

		const std::string content_type = "content-type: text/html";

		std::string::size_type content_type_html = head.find(content_type);
		if (content_type_html != std::string::npos)
		{
			body = document.substr(headerend, document.size());
			std::cout << "Valid link: " << CrawledURI << std::endl;
		}
		else
		{
			std::cout << "Invalid link: " << CrawledURI << std::endl;
			return;
		}

		ThywinPacket packet;
		packet.Type = DOCUMENT;
		packet.Method = PUT;
		packet.Content = NULL;
		std::shared_ptr<DocumentPacket> documentPacket(new DocumentPacket);
		documentPacket->Document = body;
		documentPacket->URI = CrawledURI;
		communication.SendPacket(packet);

		if (close(pagePipe[0]) == -1)
		{
			perror("Closing document pipe failed");
		}

	}

	void Crawler::startCurl(int* pageBodyPipe, const std::string& CrawledURI)
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
		{ "curl", "-i", (char *) CrawledURI.c_str(), NULL };

		if (execvp(exec[0], exec) == -1)
		{
			perror("Execute failed");
			exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);
	}

}
