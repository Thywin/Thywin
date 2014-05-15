/*
 * main.hpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas, Bobby
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "crawler.h"

using namespace thywin;

int main(int argc, char** argv)
{
	int numberOfClients = 10;

	if (argc > 2)
	{
		std::cout << "Usage: ./crawler [numberOfClients]" << std::endl;
	}

	if (argc == 2)
	{
		numberOfClients = atoi(argv[1]);
	}

	bool running = true;
	const std::string ipaddress = "192.168.100.11";

	Crawler crawler = Crawler(ipaddress);

	for (int i = 0; i < numberOfClients; i++)
	{
		pid_t pid = fork();
		if (pid == -1)
		{
			perror("Preforking failed");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)
		{
			break;
		}
	}

	while (running)
	{
		crawler.CrawlURI();
	}
	return EXIT_SUCCESS;
}

