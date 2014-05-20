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
	DatabaseHandler Master::DBConnection("192.168.100.13", 5432);

	void Master::InitializeMaster()
	{
		sem_init(&documentQueueNotEmpty, 0, 0);
		DBConnection.Connect();
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
		std::shared_ptr<URIPacket> element = DBConnection.RetrieveAndDeleteURIFromQueue();
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
		URIElement->URI =
				"http://ieeexplore.ieee.org/xpl/login.jsp?tp=&arnumber=685270&url=http%3A%2F%2Fieeexplore.ieee.org%2Fiel4%2F5611%2F15013%2F00685270.pdf%3Farnumber%3D685270\0";
		URIElement->Relevance = 0.5;
		DBConnection.AddURIToList(URIElement);
		DBConnection.AddURIToQueue(URIElement->URI);

		std::shared_ptr<URIPacket> otherElement(new URIPacket);
		otherElement->URI = "http://en.wikipedia.org/wiki/Discrete_event_simulation\0";
		otherElement->Relevance = 0.01;
		DBConnection.AddURIToList(otherElement);
		DBConnection.AddURIToQueue(otherElement->URI);

		std::shared_ptr<URIPacket> anotherElement(new URIPacket);
		anotherElement->URI = "http://www.reliasoft.com/reno/features1.htm\0";
		anotherElement->Relevance = 0.01;
		DBConnection.AddURIToList(anotherElement);
		DBConnection.AddURIToQueue(anotherElement->URI);
	}

}
