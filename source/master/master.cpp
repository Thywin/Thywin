/*
 * main.cpp
 *
 *  Created on: 23 apr. 2014
 *      Author: Thomas Kooi
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
#include "Master.h"
#include "MasterCommunicator.h"
#include "URIPacket.h"
#include "DocumentPacket.h"

namespace thywin
{
	int counterURI = 0;
	int counterDocs = 0;

	std::vector<std::shared_ptr<URIPacket>> Master::URIQueue;
	std::vector<std::shared_ptr<DocumentPacket>> Master::documentQueue;

	std::mutex Master::URIQueueMutex;
	std::mutex Master::DocumentQueueMutex;

	void Master::AddURIElementToQueue(std::shared_ptr<URIPacket> element)
	{
		printf("AddURIElementToQueue\n");
		Master::URIQueueMutex.lock();
		Master::URIQueue.insert(Master::URIQueue.end(), element);
		Master::URIQueueMutex.unlock();
	}

	std::shared_ptr<URIPacket> Master::GetNextURIElementFromQueue()
	{
		Master::URIQueueMutex.lock();

		std::shared_ptr<URIPacket> element;
		if (Master::URIQueue.size() == 0)
		{
			Master::fillURLQueue(); // Temporary queue filling for debug purposes
		}

		if (Master::URIQueue.size() > 0)
		{
			element = Master::URIQueue.at(0);
			Master::URIQueue.erase(URIQueue.begin());
		}
		else
		{
			std::shared_ptr<URIPacket> el(new URIPacket);
			el->URI = "www.google.nl\0";
			el->Relevance = 0;
			element = el;
		}
		printf("URI QUEUE: %i\n", Master::URIQueue.size());
		Master::URIQueueMutex.unlock();
		return element;
	}

	void Master::AddDocumentElementToQueue(std::shared_ptr<DocumentPacket> element)
	{
		Master::DocumentQueueMutex.lock();

		counterDocs++;
		Master::documentQueue.insert(Master::documentQueue.end(), element);

		// unset semaphore is empty
		Master::DocumentQueueMutex.unlock();
	}

	std::shared_ptr<DocumentPacket> Master::GetNextDocumentElementFromQueue()
	{
		while (Master::documentQueue.size() < 1)
		{
		}
		Master::DocumentQueueMutex.lock();
		// check semaphore is empty

		std::shared_ptr<DocumentPacket> element = NULL;
		if (Master::documentQueue.size() > 0)
		{
			element = Master::documentQueue.at(0);
			Master::documentQueue.erase(Master::documentQueue.begin());
			counterDocs--;
		}
		else
		{
		}
		if (documentQueue.size() < 1)
		{
			// set semaphore is empty
		}
		printf("DOCUMENT QUEUE: %i\n", Master::documentQueue.size());
		Master::DocumentQueueMutex.unlock();
		return element;
	}

	void Master::fillURLQueue()
	{

		std::shared_ptr<URIPacket> URIElement(new URIPacket);
		URIElement->URI =
				"http://ieeexplore.ieee.org/xpl/login.jsp?tp=&arnumber=685270&url=http%3A%2F%2Fieeexplore.ieee.org%2Fiel4%2F5611%2F15013%2F00685270.pdf%3Farnumber%3D685270\0";
		URIElement->Relevance = 0.5;
		URIQueue.insert(URIQueue.end(), URIElement);

		std::shared_ptr<URIPacket> otherElement(new URIPacket);
		otherElement->URI = "http://en.wikipedia.org/wiki/Discrete_event_simulation\0";
		otherElement->Relevance = 0.01;
		URIQueue.insert(URIQueue.end(), otherElement);

		std::shared_ptr<URIPacket> anotherElement(new URIPacket);
		anotherElement->URI = "http://www.reliasoft.com/reno/features1.htm\0";
		anotherElement->Relevance = 0.01;
		URIQueue.insert(URIQueue.end(), anotherElement);

	}

}
