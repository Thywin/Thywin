/*
 * Communicator.h
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#ifndef PARSERCOMMUNICATOR_H_
#define PARSERCOMMUNICATOR_H_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include "Document.h"
#include "URIRelevance.h"

namespace thywin
{
	const short URL = 1;
	const short DOCUMENT = 2;
	const short GET = 1;
	const short PUT = 2;

	typedef struct RequestPacket
	{
			short type;
			short action;
			unsigned int size;
	} RequestPacket;

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
			Document GetDocumentFromQueue();

			/*
			 * todo
			 */
			void StoreIndex(DocumentVector index);

			/*
			 * Store an URI with the relevance of the document the URI is found in.
			 */
			void StoreExpectedURIRelevance(URIRelevance uriRelevance);

			/*
			 * Store the actual relevance of a URI.
			 */
			void StoreActualURIRelevance(URIRelevance uriRelevance);

		private:

			enum
			{
				eDocumentQueue = 0, eURLQueue = 1, eIndexStore = 2
			};
			int masterSockets[3];
			struct sockaddr_in masterSockAddrs[3];
	};

} /* namespace thywin */

#endif /* PARSERCOMMUNICATOR_H_ */
