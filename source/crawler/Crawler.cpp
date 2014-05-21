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
		try
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
		catch (std::bad_alloc& e)
		{
			logger.Log(ERROR, "Crawler CrawlUri, Creating new URIPacket/ThywinPacket failed: " + std::string(e.what()));
		}
		catch (std::exception& e)
		{
			logger.Log(ERROR, "Crawler CrawlUri, " + std::string(e.what()));
		}
	}

	void Crawler::crawl(const std::string& URI)
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
				if (close(pagePipe[0]) == -1)
				{
					throw std::runtime_error(std::string("failed to close pipe[0]: ") + strerror(errno));
				}
				startCurl(pagePipe[1], URI);
				break;

			case -1:
				throw std::runtime_error(std::string("failing to fork process: ") + strerror(errno));

			default:
				if (close(pagePipe[1]) == -1)
				{
					logger.Log(WARNING, "Failed to close pagePipe[1]");
					throw std::runtime_error(std::string("failed to close pipe[1]: ") + strerror(errno));
				}
				sendURIDocument(pagePipe[0], URI);
		}
	}

	void Crawler::startCurl(int pagePipeWrite, const std::string& crawledURI)
	{
		if (dup2(pagePipeWrite, STDOUT_FILENO) == -1)
		{
			throw std::runtime_error(std::string("failing to dup: ") + strerror(errno));
		}

		if (close(STDERR_FILENO) == -1)
		{
			throw std::runtime_error(std::string("failing to close stderr: ") + strerror(errno));
		}

		char* exec[] =
		{ "curl", "-A", "Thywin/1.0", "-i", (char *) crawledURI.c_str(), NULL };

		execvp(exec[0], exec);
		throw std::runtime_error(std::string("failing to execute with execvp: ") + strerror(errno));
	}

	void Crawler::sendURIDocument(int pagePipeRead, const std::string& crawledURI)
	{
		char buffer;
		int readSize = read(pagePipeRead, &buffer, sizeof(buffer));

		std::string document;

		while (readSize > 0)
		{
			document.push_back(buffer);
			readSize = read(pagePipeRead, &buffer, sizeof(buffer));
		}

		if (close(pagePipeRead) == -1)
		{
			throw std::runtime_error(std::string("failed to close pipe[0]: ") + strerror(errno));
		}

		std::string::size_type headerend = document.find("\r\n\r\n");
		std::string header = document.substr(0, headerend);
		std::transform(header.begin(), header.end(), header.begin(), ::tolower);

		const std::string http_moved = "http/1.1 30";
		std::string::size_type http_moved_in_head = header.find(http_moved);
		if (http_moved_in_head != std::string::npos)
		{
			parseMovedFile(header, crawledURI);
		}
		else
		{
			const std::string content_type = "content-type: text/html";
			std::string::size_type content_type_html = header.find(content_type);
			if (content_type_html != std::string::npos)
			{
				createAndSendPacket(document, headerend, crawledURI);
			}
			else
			{
				logger.Log(INFO, "Invalid link: " + std::string(crawledURI));
			}
		}
	}

	void Crawler::parseMovedFile(const std::string& header, const std::string& crawledURI)
	{
		const std::string redirect_URI = "location: ";
		std::string::size_type head_location = header.find(redirect_URI);
		std::string::size_type head_location_end = header.find("\r\n", head_location);
		int head_start = head_location + redirect_URI.size();
		if (head_location != std::string::npos)
		{
			crawl(header.substr(head_start, head_location_end - head_start));
		}
	}

	void Crawler::createAndSendPacket(const std::string& document, const std::string::size_type& headerend,
			const std::string& crawledURI)
	{
		std::string body = document.substr(headerend, document.size());

		std::shared_ptr<DocumentPacket> documentPacket(new DocumentPacket);
		documentPacket->Document = body;
		documentPacket->URI = crawledURI;

		ThywinPacket packet;
		packet.Type = DOCUMENT;
		packet.Method = PUT;
		packet.Content = documentPacket;

		communication.SendPacket(packet);
		logger.Log(INFO, "Sent valid link: " + std::string(crawledURI));
	}
}
