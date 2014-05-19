/*
 * main.hpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas Kooi,
 *      Author: Bobby Bouwmann
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "crawler.h"
#include "Communicator.h"

using namespace thywin;

int main(int argc, char** argv)
{
	Logger logger = Logger("log");

	int NUMBER_OF_CLIENTS = 10;

	if (argc > 2)
	{
		std::cout << "Usage: ./crawler [numberOfClients]" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (argc == 2)
	{
		NUMBER_OF_CLIENTS = atoi(argv[1]);
	}

	const std::string ipaddress = "192.168.100.11";
	const int port = 7500;

	std::string message = "Starting the crawler with";
	message << "ip: " << ipaddress << " and ";
	message << "port: " << port;
	logger.Log(INFO, message);

	for (int i = 0; i < NUMBER_OF_CLIENTS - 1; i++)
	{
		pid_t processID = fork();
		if (processID == -1)
		{
			perror("Preforking failed");
			exit(EXIT_FAILURE);
		}
		else if (processID == 0)
		{
			break; // exit loop as child
		}
	}

	Crawler crawler = Crawler(ipaddress, port);

	while (true)
	{
		crawler.CrawlURI();
	}
	return EXIT_SUCCESS;
}

