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
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>


namespace thywin
{

	class Master
	{
		public:

			/**
			 * Handle for the database connection.
			 */
			static DatabaseHandler DBConnection;

			/**
			 * Initializes the master & semaphores for the Queues.
			 *
			 */
			static void InitializeMaster();

			/**
			 * Grabs an URIElement struct from the URI Queue.
			 * @return		URIElement struct.
			 */
			static std::shared_ptr<URIPacket> GetNextURIElementFromQueue();

			/**
			 * Adds a new URI element to the URI Queue.
			 * @URIElement element
			 */
			static void AddURIElementToQueue(std::shared_ptr<URIPacket> element);

			/**
			 * Grabs an documentElement struct from the Document Queue. Function call is blocking.
			 * 	Waits until the Document Queue has at least one element.
			 *
			 * @return		documentElement struct.
			 */
			static std::shared_ptr<DocumentPacket> GetNextDocumentElementFromQueue();

			/**
			 * Adds a new document element to the Document Queue.
			 * @documentElement element		Pointer to a documentElement struct
			 */
			static void AddDocumentElementToQueue(std::shared_ptr<DocumentPacket> element);

		private:

			static std::mutex URIQueueMutex;
			static std::mutex DocumentQueueMutex;
			static sem_t documentQueueNotEmpty;

			static std::vector<std::shared_ptr<URIPacket>> URIQueue;
			static std::vector<std::shared_ptr<DocumentPacket>> documentQueue;
			static void fillURLQueue();
	};
}

#endif /* MASTER_HPP_ */
