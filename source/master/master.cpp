/*
 * main.cpp
 *
 *  Created on: 23 apr. 2014
 *      Author: Thomas
 */
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>
#include "master.hpp"
#include "md5.h"
#include <fcntl.h>

struct urlQueueElement getNextURLElementFromQueue();
void addURLElementToQueue(struct urlQueueElement element);
void fillURLQueue();

struct documentQueueElement getNextDocumentElementFromQueue();
void addDocumentElementToQueue(struct documentQueueElement element);

/*
 struct requestContainer
 {
 short type;
 short action;
 int sizeOfContent;
 char* content;
 int sizeOfMeta;
 char* meta;
 };*/
struct urlQueueElement
{
	char* url;
	int hostDocumentRelevance;
};
struct documentQueueElement
{
	char* hostURL;
	char* content;
};

std::vector<urlQueueElement> urlQueue;
std::vector<documentQueueElement> documentQueue;

int main(int argc, char** argv)
{

	std::stringstream ss;

	ss << 100;
	std::cout << ss;

	// dummy fill of list

	int port = 7000;
	thywin::master master;
	int serverDesc = master.setupServer(port);
	if (serverDesc < 0)
	{
		return 1;
	}

	struct sockaddr_in client;
	int c = sizeof(client);
	//pthread_t thread_id;*/
	while (1)
	{
		int clientDesc = accept(serverDesc, (struct sockaddr *) &client,
				(socklen_t*) &c);
		if (clientDesc < 0)
		{
			perror("Failed to accept");
		}
		else
		{
			printf("Connection accepted with client: %s port %i\n",
					inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			//if (pthread_create(&thread_id, NULL, master.handleConnection,
			//		(void *) &clientDesc) < 0)
			//{
			//	perror("could not create thread");
			//	return 1;
			//}
			master.handleConnection(&clientDesc);
		}
	}
	return EXIT_SUCCESS;
}

void thywin::master::handleConnection(void *socket)
{
	int clientDesc = *(int *) socket;
	int received;
	char* reply;
	requestContainer container;
	received = recv(clientDesc, &container, sizeof(container), 0);

	//while ((received = recv(clientDesc,&container, sizeof(container), 0)) > 0)
	//
	printf("\n\nReceived bytes: %i\n", received);
	printf("Action: %i | Type: %i\n", (container.action), (container.type));

	if ((container.action) == 1)
	{
		// get
		if ((container.type) == 1)
		{
			// return url from queue
			struct urlQueueElement nextElement = getNextURLElementFromQueue();
			reply = nextElement.url;

			printf("Reply: %s\n", reply);
			if (send(clientDesc, reply, strlen(reply), 0) < 0)
				perror("Error with send");

		}
		else if ((container.type) == 2)
		{
			// return document from queue
			struct requestContainer container;
			container.action = 2; // put
			container.type = 2; // get
			documentQueueElement returnElement =
					getNextDocumentElementFromQueue();
			container.size = strlen(returnElement.hostURL);
			if (send(clientDesc, (void *) &container, sizeof(requestContainer),
					0) < 0)
				perror("Error with send; attempting to send requestContainer");

			if (send(clientDesc, returnElement.hostURL,
					strlen(returnElement.hostURL), 0) < 0)
				perror("Error with send; attempting to send hostURL");

			int fd = open(returnElement.content, O_RDWR, 0777);
			if (fd < 0)
			{
				perror("Failure");
			}

			char c;
			while (read(fd,&c,0) >0)
			{
				send(clientDesc, &c, 1, 0);
			}
			close(fd);
			unlink(returnElement.content);
		}
	}
	else if ((container.action) == 2)
	{
		if ((container.type) == 1)
		{
			struct urlQueueElement element;
			element.hostDocumentRelevance = 0;
			int sizeOfURL = (container.size);
			char url[sizeOfURL];
			int receivedURLSize = recv(clientDesc, &url, sizeof(url), 0);
			element.url = url;
			addURLElementToQueue(element);
			// place url in queue
		}
		else if ((container.type) == 2)
		{
			printf("Expected url size: %i\n", container.size);

			int sizeOfURL = (container.size);
			char url[sizeOfURL];
			int receivedURLSize = recv(clientDesc, &url, sizeof(url), 0);
			printf("received URL Size: %i\n", receivedURLSize);
			printf("Received url: %s\n", url);
			std::string newurl(url);
			const char* newerulr = md5(newurl).c_str();

			printf("Creating new file: %s\n", newerulr);
			int fd = open(newerulr, O_TRUNC | O_CREAT | O_RDWR, 0777);
			if (fd < 0)
			{
				perror("Failure");
			}

			char c;
			while (recv(clientDesc, &c, 1, 0) > 0)
			{
				write(fd, &c, 1);
			}

			printf("\nFinished writing to file\n");
			close(fd);
			//close(clientDesc);
			documentQueueElement docElement;
			docElement.hostURL = url;

			char fileName[strlen(newerulr)];
			//docElement.content = newerulr;
			strcpy(fileName, newerulr);
			docElement.content = fileName;
			documentQueue.insert(documentQueue.end(), docElement);
			printf("Added new document to queue. HostURL: %s\n",
					docElement.hostURL);
		}
	}

	if (received < 0)
		perror("error with received");

	printf("Closing connection\n");
	int closedResponse = close(clientDesc);
	if (closedResponse < 0)
		perror("error on close");
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
	urlQueue.insert(urlQueue.end(), element);
}

struct urlQueueElement getNextURLElementFromQueue()
{
	struct urlQueueElement element;
	if (urlQueue.size() == 0)
		fillURLQueue();

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
	return element;
}

void addDocumentElementToQueue(struct documentQueueElement element)
{
	documentQueue.insert(documentQueue.end(), element);
}

struct documentQueueElement getNextDocumentElementFromQueue()
{
	struct documentQueueElement element;

	if (urlQueue.size() > 0)
	{
		element = documentQueue.at(0);
		documentQueue.erase(documentQueue.begin());
	}
	else
	{
		element.content = "";
		element.hostURL = "";
	}
	return element;
}

void fillURLQueue()
{
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
	urlElement.url = "http://gd.tuwien.ac.at/languages/c/programming-bbrown/c_075.htm";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.senorenrico.nl/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "www.blackbirddevelopment.nl";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://thywin.acunote.com/projects/45558/tasks";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://thywin.com/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://premium.wpmudev.org/blog/wordpress-security-essentials-say-goodbye-to-hackers/?utm_content=buffer9dbf9&utm_medium=social&utm_source=twitter.com&utm_campaign=buffer";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://pippinsplugins.com/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "https://dribbble.com/shots/1452842-iPhone-and-Android-PSD-FREE";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://niklausgerber.com/blog/flat-ui-color-autumn-edition/?utm_content=buffer1d18f&utm_medium=social&utm_source=facebook.com&utm_campaign=buffer";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "pltts.me/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://line25.com/tutorials/create-a-cool-website-with-fancy-scrolling-effects";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.fromupnorth.com/web-design-inspiration-930/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.bluelimemedia.com/2012/03/01/build-a-custom-image-slider-using-a-custom-post-type/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://buildinternet.com/2009/05/make-an-animated-alphabet-using-keypress-events-in-jquery/";
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
	urlElement.url = "http://www.tutorialspoint.com/postgresql/postgresql_c_cpp.htm";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.benjaminarai.com/benjamin_arai/index.php?display=/eclipsecygwingcc.php";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://retis.sssup.it/~scordino/code/logger.html";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://c-mobberley.com/wordpress/2013/10/18/raspberry-pi-installation-of-postgresql-its-simple/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.cyberciti.biz/faq/linux-installing-postgresql-database-server/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.postgresql.org/docs/8.2/static/tutorial-table.html";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "https://online.han.nl/Pages/default.aspx";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://rooster.han.nl/SchoolplanFT_ICA/?";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "https://webmail.han.nl/rc/?_task=mail";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://www.microsoftstore.com/store?Action=pdp&Locale=nl_NL&SiteID=mseea&WT=srch.1&WT=mc_id.pointitsem_Microsoft+NL_google_Office+15&WT=term.microsoft+office+student&WT=campaign.Office+15&WT=content.yjAG0GtL&WT=source.google&WT=medium.cpc&cid=5371&pcrid=32675620270&pkw=microsoft+office+student&pmt=e&productID=263156100&tid=syjAG0GtL_dc";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://kickass.to/";
	urlQueue.insert(urlQueue.end(), urlElement);
	urlElement.url = "http://kickass.to/game-of-thrones-s04e05-hdtv-x264-killers-ettv-t9071446.html";
}
