/*
 * crawler.cpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas, Bobby
 */
#include <cstdio>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include "md5.h"
#include "crawler.hpp"

char* thywin::crawler::getUrl()
{
	struct sockaddr_in saddr;
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
	{
		perror("Socket failed");
	}

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(7000);
	saddr.sin_addr.s_addr = inet_addr("192.168.100.11");

	if (connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
	{
		perror("Connect failed");
	}

	requestContainer getUrlMessage;
	getUrlMessage.type = URL;
	getUrlMessage.action = GET;

	if (send(sock, (void *)&getUrlMessage, sizeof(requestContainer), 0) < 0)
	{
		perror("Send failed");
	}

	char receiveBuffer[200];
	int received = recv(sock, receiveBuffer, sizeof(receiveBuffer), 0);
	receiveBuffer[received] = '\0';

	printf("size: %d %s\n", received, receiveBuffer);
	close(sock);

	crawl(receiveBuffer);

	return receiveBuffer;
}

void thywin::crawler::sendUrlDocument(char* url, const char* documentName)
{
	struct sockaddr_in saddr;
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
	{
		perror("Socket failed");
	}

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(7000);
	saddr.sin_addr.s_addr = inet_addr("192.168.100.11");

	if (connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
	{
		perror("Connect failed");
	}

	requestContainer sendUrlDocumentMessage;
	sendUrlDocumentMessage.type = DOCUMENT;
	sendUrlDocumentMessage.action = PUT;
	sendUrlDocumentMessage.size = strlen(url);

	printf("Url Length: %s %d\n", url, strlen(url));
	printf("DocumentName: %s\n", documentName);

	if (send(sock, (void *)&sendUrlDocumentMessage, sizeof(requestContainer), 0) < 0)
	{
		perror("Send failed");
	}

	if (send(sock, url, strlen(url), 0) < 0)
	{
		perror("Send URL failed");
	}

	int fd = open(documentName, O_RDONLY);
	int readSize = 0;
	char c;
	while ((readSize = read(fd, &c, 1)) > 0) {
		if (send(sock, &c, 1, 0) < 0)
		{
			perror("Send Document failed");
		}
	}

	close(fd);
	close(sock);
}

int thywin::crawler::crawl(char* url)
{
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
	char oldurl[strlen(url)];
	strcpy(oldurl, url);
	std::string newurl(md5(url));
	const char* newerurl = newurl.c_str();

	fd = open(newerurl, O_TRUNC | O_CREAT | O_RDWR, 0777);
	if (fd < 0)
	{
		perror("Failure");
	}

	while (read(aiPY[0], &c, 1))
	{
		write(fd, &c, 1);
	}

	sendUrlDocument(oldurl, newerurl);

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

	execvp(exec[0], exec);
	perror("Exec error");

	exit(1);
}
