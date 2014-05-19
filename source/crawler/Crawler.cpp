/*
 * Crawler.cpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas Kooi, Bobby Bouwmann
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
#include <stdexcept>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <Memory>
#include "crawler.h"
#include "Communicator.h"
#include "ThywinPacket.h"
#include "URIPacket.h"
#include "DocumentPacket.h"
#include "Logger.h"

namespace thywin
{
	const std::string logfile = "log";

	Crawler::Crawler(const std::string& ipaddress, const int& port) :
			communication(ipaddress, port), logger(logfile)
	{
	}

	void Crawler::CrawlURI()
	{
		ThywinPacket packet;
		packet.Type = URI;
		packet.Method = GET;
		packet.Content = NULL;
		communication.SendPacket(packet);
		try
		{
			std::shared_ptr<URIPacket> uriPacket(new URIPacket);
			std::shared_ptr<ThywinPacket> receivedPacket(new ThywinPacket);
			receivedPacket = communication.ReceivePacket(uriPacket);

			if (receivedPacket->Type == URI && receivedPacket->Method == RESPONSE)
			{
				crawl(uriPacket->URI);
			}
		}
		catch (const std::bad_alloc& e)
		{
			logger.Log(ERROR, e.what());
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

		while (readSize > 0)
		{
			document.push_back(buffer);
			readSize = read(pagePipe[0], &buffer, sizeof(buffer));
		}

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

			try
			{
				std::shared_ptr<DocumentPacket> documentPacket(new DocumentPacket);
				documentPacket->Document = body;
				documentPacket->URI = CrawledURI;

				ThywinPacket packet;
				packet.Type = DOCUMENT;
				packet.Method = PUT;
				packet.Content = documentPacket;

				communication.SendPacket(packet);
			}
			catch(const std::exception& e)
			{
				logger.Log(ERROR, e.what());
			}
		}
		else
		{
			std::cout << "Invalid link: " << CrawledURI << std::endl;
		}

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
