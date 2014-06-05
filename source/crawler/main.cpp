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
#include <system_error>
#include <errno.h>
#include <string>
#include "Crawler.h"
#include "Communicator.h"

using namespace thywin;

int main(int argc, char** argv)
{
	Logger logger = Logger("crawler.log");

	try
	{
		if (argc != 4)
		{
			std::cerr << "Usage: " << argv[0] << " [numberOfClients] [ipaddress] [port]" << std::endl;
			exit(EXIT_FAILURE);
		}

		logger.Log(INFO, "Starting to crawl using ip: " + std::string(argv[2]));

		for (int i = 0; i < std::stoi(argv[1]); i++)
		{
			pid_t processID = fork();
			if (processID == -1)
			{
				throw std::system_error();
			}
			else if (processID == 0)
			{
				break; // exit loop as child
			}
		}

		Crawler crawler(argv[2], std::stoi(argv[3]));

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
