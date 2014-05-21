/*
 * master.hpp
 *
 *  Created on: 7 mei 2014
 *      Author: Thomas Kooi
 */

#ifndef MASTER_HPP_
#define MASTER_HPP_
#include <mutex>
#include "URIPacket.h"
#include "DocumentPacket.h"
#include "DatabaseHandler.h"
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

namespace thywin
{

	/**
	 * A vector of shared pointers to a URIPacket.
	 */
	//typedef std::vector<std::shared_ptr<URIPacket>> URIPacketSPtrs;

	class Master
	{
		public:


			/**
			 * Initializes the master & semaphores for the Queues.
			 */
			static void InitializeMaster();

			/**
			 * Grabs an URIElement struct from the URI Queue.
			 * @return shared pointer to URIPacket
			 */
			static std::shared_ptr<URIPacket> GetNextURIElementFromQueue();

			/**
			 * Adds a new URI element to the URI Queue.
			 * @param URIElement
			 */
			static void AddURIElementToQueue(std::shared_ptr<URIPacket> element);

			/**
			 * Grabs an documentElement struct from the Document Queue. Function call is blocking.
			 * Waits until the Document Queue has at least one element.
			 *
			 * @return shared pointer to DocumentPacket
			 */
			static std::shared_ptr<DocumentPacket> GetNextDocumentElementFromQueue();

			/**
			 * Adds a new document element to the Document Queue.
			 * @param documentElement Pointer to a documentElement struct
			 */
			static void AddDocumentElementToQueue(std::shared_ptr<DocumentPacket> element);

		private:

			/**
			 * Database connection object. Used get URIs or documents from the Database.
			 */
			static DatabaseHandler DBConnection;
			static std::mutex URIQueueMutex;
			static std::mutex DocumentQueueMutex;
			static sem_t documentQueueNotEmpty;
			static std::vector<std::shared_ptr<URIPacket>> URIQueue;

			/**
			 * This function will fill the URI queue with basic URIs for starting points.
			 */
			static void fillURLQueue();
	};
}

#endif /* MASTER_HPP_ */
