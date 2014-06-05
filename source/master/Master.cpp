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
	Logger Master::logger("master.queue.log");

	void Master::InitializeMaster()
	{
		sem_init(&documentQueueSemaphore, 0, DBConnection.GetRowCount("document_queue"));

		std::stringstream logMessageQueueSize;
		logMessageQueueSize << "Current document Queue size: " << DBConnection.GetRowCount("document_queue")
				<< std::endl << "Current uri Queue size: " << DBConnection.GetRowCount("uri_queue");
		logger.Log(INFO, logMessageQueueSize.str());
	}

	Master::~Master()
	{
		DBConnection.Disconnect();
	}

	void Master::AddURIElementToQueue(std::shared_ptr<URIPacket> element)
	{
		URIQueueMutex.lock();
		DBConnection.AddURIToList(element);
		URIQueueMutex.unlock();
	}

	std::shared_ptr<URIPacket> Master::GetNextURIElementFromQueue()
	{
		URIQueueMutex.lock();

		if (DBConnection.IsQueueEmpty("uri_queue"))
		{
			fillURLQueue();
			logger.Log(INFO, "Filled the uri queue with the default uri's");
			/* Fills the queue with the default starting point uri's. */
		}

		if (URIQueue.empty())
		{
			std::vector<std::shared_ptr<URIPacket>> receivedCache = DBConnection.GetURIListFromQueue(URI_QUEUE_SIZE);
			URIQueue.insert(URIQueue.end(), receivedCache.begin(), receivedCache.end());
		}
		std::shared_ptr<URIPacket> element = URIQueue.at(0);
		URIQueue.erase(URIQueue.begin());

		URIQueueMutex.unlock();
		return element;
	}

	void Master::AddDocumentElementToQueue(std::shared_ptr<DocumentPacket> element)
	{
		DocumentQueueMutex.lock();
		DBConnection.AddDocumentToQueue(element);
		sem_post(&documentQueueSemaphore);
		DocumentQueueMutex.unlock();
	}

	std::shared_ptr<DocumentPacket> Master::GetNextDocumentElementFromQueue()
	{
		sem_wait(&documentQueueSemaphore);
		DocumentQueueMutex.lock();
		std::shared_ptr<DocumentPacket> element = DBConnection.RetrieveAndDeleteDocumentFromQueue();
		DocumentQueueMutex.unlock();
		return element;
	}

	void Master::AddMultipleURISToQueue(std::shared_ptr<MultiURIPacket> packet)
	{
		for (unsigned int i = 0; i < packet->Content.size(); i++)
		{
			AddURIElementToQueue(packet->Content.at(i));
		}
	}

	void Master::PutDocumentVector(std::shared_ptr<DocumentVectorPacket> documentVector)
	{
		DocumentVectorMutex.lock();
		std::shared_ptr<URIPacket> URIElement(new URIPacket);
		URIElement->URI = documentVector->URI;
		URIElement->Relevance = documentVector->Relevance;
		DBConnection.UpdateURIInList(URIElement);
		DBConnection.AddIndex(documentVector->URI, documentVector->Index);
		DocumentVectorMutex.unlock();
	}

	void Master::fillURLQueue()
	{
		fillURIElementToQueue("http://www.cs.mun.ca/~donald/msc/node11.html");
		fillURIElementToQueue("http://mdm.sagepub.com/content/32/5/701.full");
		fillURIElementToQueue("http://en.wikipedia.org/wiki/Discrete_event_simulation");
		fillURIElementToQueue("http://www.albrechts.com/mike/DES/");
	}

	void Master::fillURIElementToQueue(const std::string& URI)
	{
		std::shared_ptr<URIPacket> packet(new URIPacket);
		packet->URI = URI;
		packet->Relevance = 0;

		DBConnection.AddURIToList(packet);
		DBConnection.AddURIToQueue(packet->URI);
	}
}
