#include <stdlib.h>
#include <stdio.h>
#include "crawler.hpp"

int main()
{
	char* collection[] = {
		"nu.nl",
		"www.google.nl",
		"http://buried.com/",
		"http://www.zedwood.com/article/cpp-md5-function",
		"http://bobobobo.wordpress.com/2010/10/17/md5-c-implementation/",
		"https://www.hackthissite.org/articles/read/1078"
	};

	thywin::crawler crawler;
	for (unsigned int i = 0; i < 6; i++)
	{
		printf("Starting collection: %d\n", i);
		if (crawler.crawl(collection[i]) < 0)
		{
			printf("Process failed!\n");
		}
	}

	return EXIT_SUCCESS;
}

