/*
 * main.cpp
 *
 *  Created on: 23 apr. 2014
 *      Author: Thomas
 */

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <mutex>
#include <fstream>
#include "master.hpp"
#include "md5.h"

std::mutex URLQueueMutex;
std::mutex DocumentQueueMutex;

struct urlQueueElement getNextURLElementFromQueue();
void addURLElementToQueue(struct urlQueueElement element);
void fillURLQueue();

struct documentQueueElement getNextDocumentElementFromQueue();
void addDocumentElementToQueue(struct documentQueueElement* element);
void *awaitClient(void *socket);

struct urlQueueElement
{
	char* url;
	int hostDocumentRelevance;
};
struct documentQueueElement
{
	std::string hostURL;
	std::string content;
};

std::vector<urlQueueElement> urlQueue;
std::vector<documentQueueElement*> documentQueue;

int main()
{

	int port = 7000;
	thywin::master master;
	int serverDesc = master.setupServer(port);
	if (serverDesc < 0)
	{
		return 1;
	}

	//struct sockaddr_in client;
	//int c = sizeof(client);

	pthread_t thread_id;
	for (int i = 0; i < 10; i++)
	{
		if (pthread_create(&thread_id, NULL, awaitClient, (void *) &serverDesc)
				< 0)
		{
			perror("could not create thread");
			return 1;
		}
	}

	while (1)
	{
	}

	return EXIT_SUCCESS;
}

void *awaitClient(void *socket)
{
	printf("STARTING NEW THREAD\n");
	struct sockaddr_in client;
	int serverDesc = *(int *) socket;
	int clientDesc;
	int c = sizeof(client);
	thywin::master master;
	while ((clientDesc = accept(serverDesc, (struct sockaddr *) &client,
			(socklen_t*) &c)))
	{
		printf("Connection accepted with client: %s port %i\n",
				inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		master.handleConnection(&clientDesc);
	}
	printf("ENDED THREAD\n");
	return (void *)0;
}

int* thywin::master::startNewConnectionThread(void *socket)
{

	int returnValue = 0;
	handleConnection(socket);
	return &returnValue;
}
void thywin::master::handleGetURL(int socket)
{
	char* reply;
	struct urlQueueElement nextElement = getNextURLElementFromQueue();
	reply = nextElement.url;
	printf("Reply: %s\n", reply);
	if (send(socket, reply, strlen(reply), 0) < 0)
	{
		perror("Error with send");
	}
}

void thywin::master::handleGetDocument(int socket)
{
	documentQueueElement returnElement = getNextDocumentElementFromQueue();
	struct RequestPacket container =
	{ 2, 2, returnElement.hostURL.size() };
	if (send(socket, (void *) &container, sizeof(RequestPacket), 0) < 0)
	{
		perror("Error with send; attempting to send requestContainer");
		return;
	}
	printf("Sending: URL: %s \nCONTENT: %s \n", returnElement.hostURL.c_str(),
			returnElement.content.c_str());
	if (send(socket, returnElement.hostURL.c_str(),
			returnElement.hostURL.size(), 0) < 0)
	{
		perror("Error with send; attempting to send hostURL");
		return;
	}
	if (!returnElement.content.empty())
	{
		printf("Opening file: %s\n", returnElement.content.c_str());
		const char* fileName = returnElement.content.c_str();
		int fd = open(fileName, O_RDONLY);
		if (fd < 0)
		{
			perror("Failure");
		}
		else
		{
			printf("file opened: %s\n", fileName);
		}
		char c;
		while (read(fd, &c, 1) > 0)
		{
			//printf("%c", c);
			if (send(socket, &c, 1, 0) < 0)
			{
				perror("It went wrong while sending");
				close(fd);
				return;
			}
		}
		close(fd);
		printf("Finished reading/sending: %s\n", fileName);
		unlink(returnElement.content.c_str());
		printf("file deleted: %s\n", fileName);
	}
	else
	{
		char c = '\0';
		send(socket, &c, 1, 0);
	}

}

void thywin::master::handlePutURL(int socket, RequestPacket container)
{
	struct urlQueueElement element;
	element.hostDocumentRelevance = 0;
	int sizeOfURL = (container.size);
	char url[sizeOfURL];
	if (recv(socket, &url, sizeof(url), 0) < 0)
	{
		perror("Erorr while receiving a URL");
	}
	element.url = url;
	addURLElementToQueue(element);
}

void thywin::master::handlePutDocument(int socket, RequestPacket container)
{
	printf("Expected URL size: %i\n", container.size);
	int sizeOfURL = (container.size);
	char url[sizeOfURL];
	int receivedURLSize = recv(socket, &url, sizeof(url), 0);
	printf("received URL Size: %i\n", receivedURLSize);
	printf("Received URL: %s\n", url);
	std::string urlConverted(url);
	const char* hashedURL = md5(urlConverted).c_str();

	printf("Creating new file: %s\n", hashedURL);
	int fd = open(hashedURL, O_TRUNC | O_CREAT | O_RDWR, 0777);
	if (fd < 0)
	{
		perror("Failure");
	}

	char c;
	while ((recv(socket, &c, 1, 0)) > 0)
	{
		//printf("%c", c);
		write(fd, &c, 1);
	}
	printf("\nFinished writing to file\n");
	close(fd);
	//close(clientDesc);
	documentQueueElement* docElement = new documentQueueElement();
	docElement->hostURL = url;

	char fileName[strlen(hashedURL)];
	//docElement.content = newerulr;
	strcpy(fileName, hashedURL);
	docElement->content = fileName;
	//documentQueue.insert(documentQueue.end(), docElement);
	printf("Added new document to queue. HostURL: %s Content: %s\n",
			docElement->hostURL.c_str(), docElement->content.c_str());
	addDocumentElementToQueue(docElement);
}

void thywin::master::handleConnection(void *socket)
{
	int clientDesc = *(int *) socket;
	RequestPacket requestPacket;
	int received = recv(clientDesc, &requestPacket, sizeof(requestPacket), 0);
	if (received < 0)
	{
		perror("Error while receiving container");
		return;
	}
	printf("\n\nReceived bytes: %i\n", received);
	printf("Action: %i | Type: %i\n", (requestPacket.action),
			(requestPacket.type));

	if ((requestPacket.action) == GET)
	{
		if ((requestPacket.type) == URL)
		{
			handleGetURL(clientDesc);
		}
		else if ((requestPacket.type) == DOCUMENT)
		{
			handleGetDocument(clientDesc);
		}
	}
	else if ((requestPacket.action) == PUT)
	{
		if ((requestPacket.type) == URL)
		{
			handlePutURL(clientDesc, requestPacket);
		}
		else if ((requestPacket.type) == DOCUMENT)
		{
			handlePutDocument(clientDesc, requestPacket);
		}
	}
	printf("Closing connection\n");
	int closedResponse = close(clientDesc);
	if (closedResponse < 0)
	{
		perror("error on close");
	}
}

int thywin::master::setupServer(int port)
{
	struct sockaddr_in server;
	int serverDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverDesc < 0)
	{
		perror("Coudln't set up socket");
		return -1;
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	int on = 1;
	setsockopt(serverDesc, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (bind(serverDesc, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		perror("Couldn't bind socket");
		return -1;
	}
	if (listen(serverDesc, 128) < 0)
	{
		perror("Couldn't listen");
		return -1;
	}
	return serverDesc;
}

void addURLElementToQueue(struct urlQueueElement element)
{
	URLQueueMutex.lock();
	urlQueue.insert(urlQueue.end(), element);
	URLQueueMutex.unlock();
}

struct urlQueueElement getNextURLElementFromQueue()
{
	URLQueueMutex.lock();
	struct urlQueueElement element;
	if (urlQueue.size() == 0)
	{
		fillURLQueue();
	}

	if (urlQueue.size() > 0)
	{
		element = urlQueue.at(0);
		urlQueue.erase(urlQueue.begin());
	}
	else
	{

		element.url = "www.google.nl\0";
		element.hostDocumentRelevance = 0;
	}
	URLQueueMutex.unlock();
	return element;
}

void addDocumentElementToQueue(struct documentQueueElement* element)
{
	DocumentQueueMutex.lock();
	printf("Putting element to queue: %s %s\n", element->content.c_str(),
			element->hostURL.c_str());
	documentQueue.insert(documentQueue.end(), element);
	printf("\n");
	for (int i = 0; i < documentQueue.size(); i++)
	{
		documentQueueElement* el = documentQueue.at(i);
		printf("ELEMENT IN QUEUE: %s %s\n", el->content.c_str(),
				el->hostURL.c_str());
	}
	printf("\n");
	DocumentQueueMutex.unlock();
}

struct documentQueueElement getNextDocumentElementFromQueue()
{
	DocumentQueueMutex.lock();
	documentQueueElement element;
	documentQueueElement* returnElement;
	if (documentQueue.size() > 0)
	{
		returnElement = documentQueue.at(0);
		element.content = returnElement->content;
		element.hostURL = returnElement->hostURL;

		//element = documentQueue.begin();
		documentQueue.erase(documentQueue.begin());
		printf("getNextDocumentElementFromQueue NON EMPTY: [%s | %s]\n",
				element.content.c_str(), element.hostURL.c_str());
	}
	else
	{
		element.content = std::string();
		element.hostURL = std::string();
		printf("getNextDocumentElementFromQueue EMPTY: %s %s\n",
				element.content.c_str(), element.hostURL.c_str());
	}

	DocumentQueueMutex.unlock();
	return element;
}

void fillURLQueue()
{
	URLQueueMutex.lock();
	struct urlQueueElement urlElement;
	urlElement.url = "www.google.nl\0";
	urlElement.hostDocumentRelevance = 0;
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "www.nu.nl\0";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "kooisoftware.nl\0";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "stackoverflow.com\0";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.thegeekstuff.com\0";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.jouwkabel.nl/nl/\0";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "www.facebook.com\0";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "www.han.nl\0";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "www.slamfm.nl\0";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://gd.tuwien.ac.at/languages/c/programming-bbrown/c_075.htm";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.senorenrico.nl/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "www.blackbirddevelopment.nl";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://thywin.acunote.com/projects/45558/tasks";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://thywin.com/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://premium.wpmudev.org/blog/wordpress-security-essentials-say-goodbye-to-hackers/?utm_content=buffer9dbf9&utm_medium=social&utm_source=twitter.com&utm_campaign=buffer";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://pippinsplugins.com/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"https://dribbble.com/shots/1452842-iPhone-and-Android-PSD-FREE";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://niklausgerber.com/blog/flat-ui-color-autumn-edition/?utm_content=buffer1d18f&utm_medium=social&utm_source=facebook.com&utm_campaign=buffer";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "pltts.me/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://line25.com/tutorials/create-a-cool-website-with-fancy-scrolling-effects";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.fromupnorth.com/web-design-inspiration-930/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://www.bluelimemedia.com/2012/03/01/build-a-custom-image-slider-using-a-custom-post-type/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://buildinternet.com/2009/05/make-an-animated-alphabet-using-keypress-events-in-jquery/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://masonry.desandro.com/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "https://github.com/Thywin/Thywin";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "https://www.youtube.com/watch?v=_sV0S8qWSy0";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "https://mail.google.com/mail/u/0/#inbox";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://en.cppreference.com/w/c";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.example-code.com/vcpp/spider.asp";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://www.tutorialspoint.com/postgresql/postgresql_c_cpp.htm";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://www.benjaminarai.com/benjamin_arai/index.php?display=/eclipsecygwingcc.php";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://retis.sssup.it/~scordino/code/logger.html";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://c-mobberley.com/wordpress/2013/10/18/raspberry-pi-installation-of-postgresql-its-simple/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://www.cyberciti.biz/faq/linux-installing-postgresql-database-server/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://www.postgresql.org/docs/8.2/static/tutorial-table.html";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "https://online.han.nl/Pages/default.aspx";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://rooster.han.nl/SchoolplanFT_ICA/?";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "https://webmail.han.nl/rc/?_task=mail";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://www.microsoftstore.com/store?Action=pdp&Locale=nl_NL&SiteID=mseea&WT=srch.1&WT=mc_id.pointitsem_Microsoft+NL_google_Office+15&WT=term.microsoft+office+student&WT=campaign.Office+15&WT=content.yjAG0GtL&WT=source.google&WT=medium.cpc&cid=5371&pcrid=32675620270&pkw=microsoft+office+student&pmt=e&productID=263156100&tid=syjAG0GtL_dc";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://kickass.to/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url =
			"http://kickass.to/game-of-thrones-s04e05-hdtv-x264-killers-ettv-t9071446.html";

	URLQueueMutex.unlock();
}
