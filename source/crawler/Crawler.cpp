/*
 * Crawler.cpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas Kooi
 *      Author: Bobby Bouwmann
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <Memory>
#include <stdexcept>
#include <errno.h>
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
			logger.Log(ERROR, "Crawler CrawlUri, Creating new URIPacket/ThywinPacket failed: " + std::string(e.what()));
		}
		catch (const std::exception& e)
		{
			logger.Log(ERROR, "Crawler crawl, " + std::string(e.what()));
			exit(EXIT_FAILURE);
		}
	}

	int Crawler::crawl(const std::string& URI)
	{
		int pagePipe[2];
		if (pipe(pagePipe) == -1)
		{
			throw std::runtime_error(std::string("failing to pipe: ") + strerror(errno));
		}

		int processID = fork();
		switch (processID)
		{
			case 0:
				try
				{
					startCurl(pagePipe, URI);
				}
				catch (const std::exception& e)
				{
					logger.Log(ERROR, "Crawler startCurl, " + std::string(e.what()));
				}
				break;

			case -1:
				throw std::runtime_error(std::string("failing to fork process: ") + strerror(errno));

			default:
				try
				{
					sendURIDocument(pagePipe, URI);
				}
				catch (const std::exception& e)
				{
					logger.Log(ERROR, "Crawler startCurl, " + std::string(e.what()));
				}
		}

		return EXIT_SUCCESS;
	}

	void Crawler::sendURIDocument(int* pagePipe, const std::string& crawledURI)
	{
		if (close(pagePipe[1]) == -1)
		{
			throw std::runtime_error(std::string("failed to close pipe[1]: ") + strerror(errno));
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

		const std::string content_type = "content-type: text/html";

		std::string::size_type content_type_html = head.find(content_type);
		if (content_type_html != std::string::npos)
		{
			try
			{
				createSendPacket(document, headerend, crawledURI);
			}
			catch (const std::exception& e)
			{
				logger.Log(ERROR,
						"Crawler createSendPacket, Creating new DocumentPacket failed: " + std::string(e.what()));
			}
		}
		else
		{
			logger.Log(INFO, "Invalid link: " + std::string(crawledURI));
		}

		if (close(pagePipe[0]) == -1)
		{
			throw std::runtime_error(std::string("failed to close pipe[0]: ") + strerror(errno));
		}
	}

	void Crawler::createSendPacket(const std::string& document, const std::string::size_type& headerend, const std::string& crawledURI)
	{
		std::string body = document.substr(headerend, document.size());

		logger.Log(INFO, "Valid link: " + std::string(crawledURI));

		std::shared_ptr<DocumentPacket> documentPacket(new DocumentPacket);
		documentPacket->Document = body;
		documentPacket->URI = crawledURI;

		ThywinPacket packet;
		packet.Type = DOCUMENT;
		packet.Method = PUT;
		packet.Content = documentPacket;

		communication.SendPacket(packet);
	}

	void Crawler::startCurl(int* pageBodyPipe, const std::string& CrawledURI)
	{
		if (close(pageBodyPipe[0]) == -1)
		{
			throw std::runtime_error(std::string("failed to close pipe[0]: ") + strerror(errno));
		}

		if (dup2(pageBodyPipe[1], STDOUT_FILENO) == -1)
		{
			throw std::runtime_error(std::string("failing to dup: ") + strerror(errno));
		}

		if (close(STDERR_FILENO) == -1)
		{
			throw std::runtime_error(std::string("failing to close stderr: ") + strerror(errno));
		}

		char* exec[] =
		{ "curl", "-i", (char *) CrawledURI.c_str(), NULL };

		execvp(exec[0], exec);
		throw std::runtime_error(std::string("failing to execute with execvp: ") + strerror(errno));
		exit(EXIT_FAILURE);
	}
}
