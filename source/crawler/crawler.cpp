/*
 * crawler.cpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas
 */
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "md5.h"
#include "crawler.hpp"

int thywin::crawler::crawl(char* url)
{
	printf("Start from crawl: %s\n", url);
	int aiPY[2];
	pipe(aiPY);

	switch (fork())
	{
	case 0:
		crawler::Child( aiPY, url);
		break;

	case -1:
		perror("Can\'t create child!");
		return -1;

	default:
		crawler::Parent(aiPY, url);
	}

	return 0;
}

void thywin::crawler::Parent(int* aiPY, char* url)
{
	char c;
	int fd;
	close(aiPY[1]);

	std::string newurl(url);
	const char* newerulr = md5(newurl).c_str();

	printf("Creating new file: %s (PARENT)\n", newerulr);
	fd = open(newerulr, O_TRUNC | O_CREAT | O_RDWR, 0777);
	if (fd < 0)
	{
		perror("Failure");
	}
	while (read(aiPY[0], &c, 1))
		write(fd, &c, 1);
	printf("Finished writing to file (PARENT)\n");
	close(aiPY[0]);

	close(fd);

	wait(0);
}

void thywin::crawler::Child(int* aiPY, char* url)
{

	close(aiPY[0]);

	close(1);
	dup(aiPY[1]);
	close(aiPY[1]);

	char* exec[] = {
			"wget",
			"-qO-",
			url,
			NULL
	};
	printf("Starting wget for: %s (CHILD)\n", url);
	execvp(exec[0], exec);
	perror("Exec error");
	printf("Bestandsnaam: %s", url);

	exit(1);
}
