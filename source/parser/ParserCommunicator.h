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
#include "DocumentVector.h"
#include "Communicator.h"

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
			 * @param documentQueue ipaddress of the documentQueue
			 * @param URIQueue ipaddress of the URIQueue
			 * @param indexStore ipaddress of the indexStore
			 */
			ParserCommunicator(std::string documentQueue, std::string URIQueue, std::string indexStore);
			virtual ~ParserCommunicator();

			/*
			 * Get a document from the Document Queue 
			 */
			DocumentPacket GetDocumentFromQueue();

			/*
			 * todo
			 */
			void StoreIndex(DocumentVector index);

			/*
			 * Store an URI with the relevance of the document the URI is found in.
			 */
			void StoreExpectedURIRelevance(URIPacket uriPacket);

			/*
			 * Store the actual relevance of a URI.
			 */
			void StoreActualURIRelevance(URIPacket uriPacket);

		private:
			Communicator communicator;
	};

} /* namespace thywin */

#endif /* PARSERCOMMUNICATOR_H_ */
