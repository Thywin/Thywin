/*
 * main.cpp
 *
 *  Created on: 23 apr. 2014
 *      Author: Thomas Kooi
 */

#include <stdlib.h>
#include <string>
#include <vector>
#include <mutex>
#include <errno.h>
#include "Master.h"
#include "MasterCommunicator.h"
#include "URIPacket.h"
#include "DocumentPacket.h"
#include <semaphore.h>
#include "DatabaseHandler.h"
#include <algorithm>

namespace thywin
{
	std::mutex Master::URIQueueMutex;
	std::mutex Master::DocumentQueueMutex;
	sem_t Master::documentQueueNotEmpty;
	std::vector<std::shared_ptr<URIPacket>> Master::URIQueue;
	DatabaseHandler Master::DBConnection("192.168.100.13", 5432);

	void Master::InitializeMaster()
	{
		DBConnection.Connect();
		sem_init(&documentQueueNotEmpty, 0, DBConnection.GetRowCount("document_queue"));
		printf("Current document Queue size: %i\n", DBConnection.GetRowCount("document_queue"));
		printf("Current uri Queue size: %i\n", DBConnection.GetRowCount("uri_queue"));
	}

	void Master::AddURIElementToQueue(std::shared_ptr<URIPacket> element)
	{
		Master::URIQueueMutex.lock();
		DBConnection.AddURIToList(element);
		DBConnection.AddURIToQueue(element->URI);
		Master::URIQueueMutex.unlock();
	}

	std::shared_ptr<URIPacket> Master::GetNextURIElementFromQueue()
	{
		Master::URIQueueMutex.lock();

		if (DBConnection.IsQueueEmpty("uri_queue"))
		{
			Master::fillURLQueue();
			/* Temporary queue filling for debug purposes. */
		}

		if (URIQueue.empty())
		{
			printf("URIQueue is empty. Start to refill from database\n");
			std::vector<std::shared_ptr<URIPacket>> receivedCache = DBConnection.GetURIListFromQueue();
			Master::URIQueue.insert(Master::URIQueue.end(), receivedCache.begin(), receivedCache.end());
			printf("URIQueue refilled with new elements: %i\n",receivedCache.size());
		}
		std::shared_ptr<URIPacket> element = Master::URIQueue.at(0);
		Master::URIQueue.erase(URIQueue.begin());

		Master::URIQueueMutex.unlock();
		return element;
	}

	void Master::AddDocumentElementToQueue(std::shared_ptr<DocumentPacket> element)
	{
		Master::DocumentQueueMutex.lock();
		DBConnection.AddDocumentToQueue(element);
		sem_post(&documentQueueNotEmpty);
		Master::DocumentQueueMutex.unlock();
	}

	std::shared_ptr<DocumentPacket> Master::GetNextDocumentElementFromQueue()
	{
		sem_wait(&documentQueueNotEmpty);
		Master::DocumentQueueMutex.lock();
		std::shared_ptr<DocumentPacket> element = DBConnection.RetrieveAndDeleteDocumentFromQueue();
		Master::DocumentQueueMutex.unlock();
		return element;
	}

	void Master::fillURLQueue()
	{

		std::shared_ptr<URIPacket> URIElement(new URIPacket);
		URIElement->URI = "http://thywin.com/kaas/kaas/index.html\0";
		URIElement->Relevance = 0.5;
		DBConnection.AddURIToList(URIElement);
		DBConnection.AddURIToQueue(URIElement->URI);

		std::shared_ptr<URIPacket> newelemente(new URIPacket);
		newelemente->URI = "http://www.nu.nl\0";
		newelemente->Relevance = 0.01;
		DBConnection.AddURIToList(newelemente);
		DBConnection.AddURIToQueue(newelemente->URI);

		std::shared_ptr<URIPacket> anotherElement(new URIPacket);
		anotherElement->URI = "http://www.reliasoft.com/reno/features1.htm\0";
		anotherElement->Relevance = 0.01;
		DBConnection.AddURIToList(anotherElement);
		DBConnection.AddURIToQueue(anotherElement->URI);

		std::shared_ptr<URIPacket> otherElement(new URIPacket);
		otherElement->URI = "http://en.wikipedia.org/wiki/Discrete_event_simulation\0";
		otherElement->Relevance = 0.01;
		DBConnection.AddURIToList(otherElement);
		DBConnection.AddURIToQueue(otherElement->URI);

	}

}
