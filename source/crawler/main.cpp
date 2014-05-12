/*
 * main.hpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas, Bobby
 */

#include <stdlib.h>
#include <stdio.h>
#include "crawler.h"

int main()
{
	const std::string ipaddress = "192.168.100.11";

	thywin::Crawler crawler = thywin::Crawler(ipaddress);

	for (;;) // Loop forever!
	{
		crawler.CrawlUri();
	}
	return EXIT_SUCCESS;
}

