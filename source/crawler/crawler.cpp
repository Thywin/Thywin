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
#include <fstream>
#include <sstream>
#include "md5.h"
#include "crawler.hpp"

void thywin::crawler::crawlUrl()
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

	std::string received;
	char buffer;
	while(recv(sock, &buffer, sizeof(buffer), 0) > 0)
	{
		received.push_back(buffer);
	}

	std::cout << "size: " << received.size() << "\nBuffer: " << received << std::endl;
	close(sock);

	crawl(received);
}

void thywin::crawler::sendUrlDocument(std::string url, std::string documentName, int* aiPY)
{
	close(aiPY[1]);
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
	sendUrlDocumentMessage.size = url.size();

	std::cout << "Url: " << url << " Length: " << url.size() << " DocumentName: " << documentName << std::endl;

	if (send(sock, (void *)&sendUrlDocumentMessage, sizeof(requestContainer), 0) < 0)
	{
		perror("Send failed");
	}

	const char* charurl = url.c_str();
	if (send(sock, charurl, strlen(charurl), 0) < 0)
	{
		perror("Send URL failed");
	}

	int readSize = 0;
	char c;
	while ((readSize = read(aiPY[0], &c, sizeof(c))) > 0) {
		if (send(sock, &c, 1, 0) < 0)
		{
			perror("Send Document failed");
		}
	}

	close(aiPY[0]);
	close(sock);
}

int thywin::crawler::crawl(std::string url)
{
	int aiPY[2];
	pipe(aiPY);

	switch (fork())
	{
		case 0:
			crawler::Child(aiPY, url);
			break;

		case -1:
			perror("Can\'t create child!");
			return -1;

		default:
			crawler::Parent(aiPY, url);
	}
	return 0;
}

void thywin::crawler::Parent(int* aiPY, std::string url)
{
	std::string documentName = md5(url);
	sendUrlDocument(url, documentName, aiPY);

	wait(0);
}

void thywin::crawler::Child(int* aiPY, std::string url)
{
	close(aiPY[0]);
	close(1);
	dup(aiPY[1]);
	close(aiPY[1]);

	const char* newurl = url.c_str();
	char* newerurl;
	strcpy(newerurl, newurl);

	char* exec[] = {
		"wget",
		"-qO-",
		newerurl,
		NULL
	};

	execvp(exec[0], exec);
	perror("Exec error");

	exit(1);
}
