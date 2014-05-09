/*
 * main.hpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas, Bobby
 */

#include <stdlib.h>
#include <stdio.h>
#include "crawler.hpp"

int main()
{
	thywin::crawler crawler;

	for (unsigned int i = 0; i < 100; i++)
	{
		crawler.crawlUrl();
	}
	return EXIT_SUCCESS;
}

