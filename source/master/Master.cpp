/*
 * main.cpp
 *
 *  Created on: 23 apr. 2014
 *      Author: Thomas Kooi
 *      Author: Thomas Gerritsen
 */

#include <stdlib.h>
#include <string>
#include <vector>
#include <mutex>
#include <errno.h>
#include <algorithm>
#include <semaphore.h>
#include "Master.h"
#include "MasterCommunicator.h"
#include "URIPacket.h"
#include "DocumentPacket.h"
#include "DatabaseHandler.h"

namespace thywin
{
	std::mutex Master::URIQueueMutex;
	std::mutex Master::DocumentQueueMutex;
	std::mutex Master::DocumentVectorMutex;
	sem_t Master::documentQueueSemaphore;
	std::vector<std::shared_ptr<URIPacket>> Master::URIQueue;
	DatabaseHandler Master::DBConnection;


	void Master::InitializeMaster()
	{
		sem_init(&documentQueueSemaphore, 0, DBConnection.GetRowCount("document_queue"));
		printf("Current document Queue size: %i\n", DBConnection.GetRowCount("document_queue"));
		printf("Current uri Queue size: %i\n", DBConnection.GetRowCount("uri_queue"));
	}

	Master::~Master()
	{
		DBConnection.Disconnect();
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
		try
		{
			if (DBConnection.IsQueueEmpty("uri_queue"))
			{
				Master::fillURLQueue();
				/* Temporary queue filling for debug purposes. */
			}
		}
		catch (std::exception& e)
		{
			// To be added to log once library is updated
		}
		if (URIQueue.empty())
		{
			std::vector<std::shared_ptr<URIPacket>> receivedCache = DBConnection.GetURIListFromQueue(URI_QUEUE_SIZE);
			Master::URIQueue.insert(Master::URIQueue.end(), receivedCache.begin(), receivedCache.end());
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
		sem_post(&documentQueueSemaphore);
		Master::DocumentQueueMutex.unlock();
	}

	std::shared_ptr<DocumentPacket> Master::GetNextDocumentElementFromQueue()
	{
		sem_wait(&documentQueueSemaphore);
		Master::DocumentQueueMutex.lock();
		std::shared_ptr<DocumentPacket> element = DBConnection.RetrieveAndDeleteDocumentFromQueue();
		Master::DocumentQueueMutex.unlock();
		return element;
	}

	void Master::PutDocumentVector(std::shared_ptr<DocumentVectorPacket> documentVector)
	{
		DocumentVectorMutex.lock();

		std::shared_ptr<URIPacket> URIElement(new URIPacket);
		URIElement->URI = documentVector->URI;
		URIElement->Relevance = documentVector->Relevance;
		DBConnection.AddURIToList(URIElement);
		for (DocumentVector::iterator i = documentVector->Index.begin(); i != documentVector->Index.end(); i++)
		{
			DBConnection.AddWordcountToIndex(documentVector->URI, i->first, i->second);
		}
		DocumentVectorMutex.unlock();
	}

	void Master::fillURLQueue()
	{
		try
		{
			std::shared_ptr<URIPacket> URIElement(new URIPacket);
			URIElement->URI = "http://thywin.com/kaas/kaas/index.html\0";
			URIElement->Relevance = 0;
			DBConnection.AddURIToList(URIElement);
			DBConnection.AddURIToQueue(URIElement->URI);

			std::shared_ptr<URIPacket> newelemente(new URIPacket);
			newelemente->URI = "http://www.cs.mun.ca/~donald/msc/node11.html\0";
			newelemente->Relevance = 0;
			DBConnection.AddURIToList(newelemente);
			DBConnection.AddURIToQueue(newelemente->URI);

			std::shared_ptr<URIPacket> anotherElement(new URIPacket);
			anotherElement->URI = "https://www.facebook.com\0";
			anotherElement->Relevance = 0;
			DBConnection.AddURIToList(anotherElement);
			DBConnection.AddURIToQueue(anotherElement->URI);

			std::shared_ptr<URIPacket> otherElement(new URIPacket);
			otherElement->URI = "http://en.wikipedia.org/wiki/Discrete_event_simulation\0";
			otherElement->Relevance = 0;
			DBConnection.AddURIToList(otherElement);
			DBConnection.AddURIToQueue(otherElement->URI);
		}
		catch (std::bad_alloc& e)
		{
			// To be added to logger once library is updated
		}
	}

}
