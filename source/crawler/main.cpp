/*
 * main.hpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas Kooi
 *      Auhtor: Bobby Bouwmann
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <errno.h>
#include <string.h>
#include "crawler.h"
#include "Communicator.h"

using namespace thywin;

int main(int argc, char** argv)
{
	Logger logger = Logger("log");

	try
	{
		long int NUMBER_OF_CLIENTS = 10;
		std::string ipaddress = "192.168.100.13";
		long int port = 7500;

		if (argc > 4)
		{
			std::cout << "Usage: ./crawler [numberOfClients] [ipaddress] [port]" << std::endl;
			exit(EXIT_FAILURE);
		}

		if (argc == 4)
		{
			NUMBER_OF_CLIENTS = strtol(argv[1], NULL, 0);
			ipaddress = argv[2];
			port = strtol(argv[3], NULL, 0);
		} else if (argc == 2)
		{
			NUMBER_OF_CLIENTS = strtol(argv[1], NULL, 0);
		}

		logger.Log(INFO, "Starting to crawl using ip: " + std::string(ipaddress));

		for (int i = 0; i < NUMBER_OF_CLIENTS - 1; i++)
		{

			pid_t processID = fork();
			if (processID == -1)
			{
				throw std::runtime_error(std::string(strerror(errno)));
			}
			else if (processID == 0)
			{
				break; // exit loop as child
			}
		}

		Crawler crawler(ipaddress, port);

		while (true)
		{
			crawler.CrawlURI();
		}
	}
	catch (std::exception& e)
	{
		logger.Log(ERROR, "Crawler: " + std::string(e.what()));
	}
	catch (...)
	{
		logger.Log(ERROR, "Crawler: Unexpected error while crawling");
	}

	return EXIT_FAILURE;
}
