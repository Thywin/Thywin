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
}RequestPacket;

class ParserCommunicator
{
public:
	ParserCommunicator(std::string documentQueue, std::string URLQueue,
			std::string indexStore);
	virtual ~ParserCommunicator();

	Document GetDocumentFromQueue();
	void StoreIndex();
	void StoreExpectedURIRelevance(URIRelevance uriRelevance);
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
