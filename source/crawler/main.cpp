#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "md5.h"
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

	for (unsigned int i = 0; i < 6; i++)
	{
		printf("Starting collection: %d\n", i);
		if (crawler::crawl(collection[i]) < 0)
		{
			printf("Process failied!\n");
		}
	}

	return EXIT_SUCCESS;
}

int crawler::crawl(char* url)
{
	printf("Start from crawl: %s\n",url);
    int iPID, aiPX[2], aiPY[2];

    pipe(aiPX);
    pipe(aiPY);

    switch(iPID = fork())
    {
        case 0:
            crawler::Child(aiPX, aiPY, url);
            break;

        case -1:
            perror("Can\'t create child!");
            return -1;

        default:
            crawler::Parent(aiPX, aiPY, url);
    }

    return 0;
}

void crawler::Parent(int* aiPX, int* aiPY, char* url) {
    char c;
    int fd;

    close(aiPX[0]);
    close(aiPY[1]);

    std::string newurl(url);
    const char* newerulr = md5(newurl).c_str();

    printf("Creating new file: %s (PARENT)\n",newerulr);
    fd = open(newerulr, O_TRUNC | O_CREAT | O_RDWR, 0777);
    if (fd < 0) {
    	perror("Failure");
    }
    while(read(aiPY[0], &c, 1))
        write(fd, &c, 1);
    printf("Finished writing to file (PARENT)\n");
    close(aiPY[0]);

    close(fd);

    wait(0);
}

void crawler::Child(int* aiPX, int* aiPY, char* url) {
    close(aiPX[1]);
    close(aiPY[0]);

    close(0);
    dup(aiPX[0]);
    close(aiPX[0]);

    close(1);
    dup(aiPY[1]);
    close(aiPY[1]);

    char* exec[4];
	exec[0] = "wget";
	exec[1] = "-qO-";
	exec[2] = url;
    exec[3] = NULL;
    printf("Starting wget for: %s (CHILD)\n",url);
    execvp(exec[0], exec);
    perror("Exec error");

    printf("Bestandsnaam: %s", url);

    exit(1);
}
