/*
 * master.hpp
 *
 *  Created on: 7 mei 2014
 *      Author: Thomas Kooi
 *      Author: Thomas Gerritsen
 */

#ifndef MASTER_HPP_
#define MASTER_HPP_
#include <mutex>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "URIPacket.h"
#include "DocumentPacket.h"
#include "DocumentVectorPacket.h"
#include "DatabaseHandler.h"
#include "MultiURIPacket.h"
#include "Logger.h"

namespace thywin
{
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
			 * @param URIElement A shared pointer to an URI packet
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
			 * @param documentElement Pointer to a documentElement
			 */
			static void AddDocumentElementToQueue(std::shared_ptr<DocumentPacket> element);

			/**
			 * Adds multiple URIs to the URIs queue
			 * @param packet Pointer to a MultiURIPacket object
			 */
			static void AddMultipleURISToQueue(std::shared_ptr<MultiURIPacket> packet);

			/**
			 * Adds a new documentVector element to the index database.
			 * @param documentVector Pointer to a documentVector
			 */
			static void PutDocumentVector(std::shared_ptr<DocumentVectorPacket> documentVector);

			/**
			 * Disconnects the connection with the database
			 */
			virtual ~Master();

		private:

			static Logger logger;

			static const int URI_QUEUE_SIZE = 50;
			/**
			 * Database connection object. Used get URIs or documents from the Database.
			 */
			static DatabaseHandler DBConnection;

			static std::mutex URIQueueMutex;
			static std::mutex DocumentQueueMutex;
			static std::mutex DocumentVectorMutex;
			/**
			 * Semaphore for the document Queue.
			 * Will be initialized with the number of documents available in the Database Queue on start.
			 */
			static sem_t documentQueueSemaphore;
			static std::vector<std::shared_ptr<URIPacket>> URIQueue;

			/**
			 * This function will fill the URI queue with basic URIs for starting points.
			 */
			static void fillURLQueue();
			static void fillURIElementToQueue(const std::string& URI);
	};
}

#endif /* MASTER_HPP_ */
