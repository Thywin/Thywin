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
	std::vector<std::string> Master::blackListURIs;
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
		if (!uriBlackListed(element->URI))
		{
			URIQueueMutex.lock();
			DBConnection.AddURIToList(element);
			URIQueueMutex.unlock();
		}
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

	void Master::AddURIToBlackList(std::string& URI)
	{
		if (!URI.empty())
		{
			blackListURIs.insert(blackListURIs.end(), URI);
		}
	}

	void Master::fillURLQueue()
	{
		fillURIElementToQueue("http://www.cs.mun.ca/~donald/msc/node11.html");
		fillURIElementToQueue("http://mdm.sagepub.com/content/32/5/701.full");
		fillURIElementToQueue("http://en.wikipedia.org/wiki/Discrete_event_simulation");
		fillURIElementToQueue("hhttp://www.albrechts.com/mike/DES/");
	}

	void Master::fillURIElementToQueue(std::string URI)
	{
		try
		{
			std::shared_ptr<URIPacket> packet(new URIPacket);
			packet->URI = URI;
			packet->Relevance = 0;

			DBConnection.AddURIToList(packet);
			DBConnection.AddURIToQueue(packet->URI);
		}
		catch (std::bad_alloc& e)
		{
			// To be added to logger once library is updated
		}
	}

	bool Master::uriBlackListed(std::string& URI)
	{
		for (unsigned int i = 0; i < blackListURIs.size(); i++)
		{
			if (URI.find(blackListURIs.at(i)) != std::string::npos)
			{
				printf("Black listed URI found: %s triggered on %s\n", URI.c_str(), blackListURIs.at(i).c_str());
				return true;
			}
		}
		return false;
	}
}
