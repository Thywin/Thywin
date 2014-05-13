/*
 * master.hpp
 *
 *  Created on: 7 mei 2014
 *      Author: Thomas
 */

#ifndef MASTER_HPP_
#define MASTER_HPP_
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <mutex>

namespace thywin
{
	struct URIElement
	{
			std::string URI;
			int hostDocumentRelevance;
	};
	struct documentElement
	{
			std::string URI;
			std::string content;
	};

	class Master
	{
		public:
			/**
			 * Grabs an URIElement struct from the URI Queue.
			 * @return		URIElement struct.
			 */
			static URIElement GetNextURIElementFromQueue();

			/**
			 * Adds a new URI element to the URI Queue.
			 * @URIElement element
			 */
			static void AddURIElementToQueue(URIElement element);

			/**
			 * Grabs an documentElement struct from the Document Queue. Function call is blocking.
			 * 	Waits until the Document Queue has at least one element.
			 *
			 * @return		documentElement struct.
			 */
			static documentElement GetNextDocumentElementFromQueue();

			/**
			 * Adds a new document element to the Document Queue.
			 * @documentElement element		Pointer to a documentElement struct
			 */
			static void AddDocumentElementToQueue(documentElement element);

		private:

			static std::mutex URIQueueMutex;
			static std::mutex DocumentQueueMutex;
			static std::vector<URIElement> URIQueue;
			static std::vector<documentElement> documentQueue;
			static void fillURLQueue();
	};
}

#endif /* MASTER_HPP_ */
