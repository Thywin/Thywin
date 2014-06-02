/*
 * Communicator.h
 *
 *  Created on: May 8, 2014
 *      Author: Imre Woudstra
 */

#ifndef PARSERCOMMUNICATOR_H_
#define PARSERCOMMUNICATOR_H_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include "DocumentPacket.h"
#include "URIPacket.h"
#include "MultiURIPacket.h"
#include "DocumentVector.h"
#include "DocumentVectorPacket.h"
#include "Communicator.h"
#include "Logger.h"

namespace thywin
{
	/**
	 * 
	 */
	class ParserCommunicator
	{
		public:
			/*
			 * Creates a ParserCommunicator which can be used for communication with the queues
			 * @param masterIP ipaddress of the master PI
			 * @param masterPort port of the master PI
			 */
			ParserCommunicator(const std::string& masterIP, const unsigned short masterPort);

			/**
			 * Default destructor
			 */
			virtual ~ParserCommunicator();

			/*
			 * Get a document from the Document Queue 
			 */
			DocumentPacket GetDocumentFromQueue();

			/*
			 * Store an Index with the relevance and the URI of the document.
			 */
			void StoreIndex(const DocumentVectorPacket& index);

			/*
			 * Store an URI with the relevance of the document the URI is found in.
			 */
			void StoreExpectedURIRelevance(const URIPacket& uriPacket);

			/*
			 * Store the actual relevance of a URI.
			 */
			void StoreActualURIRelevance(const URIPacket& uriPacket);

			/**
			 * Store multiple uris with their relevance.
			 */
			void StoreMultipleURIs(const MultiURIPacket& multiURIPacket);

		private:
			Logger logger;
			Communicator communicator;
	};

} /* namespace thywin */

#endif /* PARSERCOMMUNICATOR_H_ */
