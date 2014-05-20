/*
 * ParserCommunicator.cpp
 *
 *  Created on: May 8, 2014
 *      Author: Imre Woudstra
 */

#include "ParserCommunicator.h"
#include "DocumentVector.h"
#include "ThywinPacket.h"
#include <string.h>

namespace thywin
{

	ParserCommunicator::ParserCommunicator(std::string documentQueue, std::string URLQueue, std::string indexStore) :
			logger("parser.log"), communicator(documentQueue, 7500)
	{
		logger.Log(INFO, std::string("Connected to the master will now start parsing!"));
	}

	ParserCommunicator::~ParserCommunicator()
	{
	}

	DocumentPacket ParserCommunicator::GetDocumentFromQueue()
	{
		std::shared_ptr<DocumentPacket> doc(new DocumentPacket);

		ThywinPacket requestPacket;
		requestPacket.Method = GET;
		requestPacket.Type = DOCUMENT;

		communicator.SendPacket(requestPacket);
		communicator.ReceivePacket(doc);

		return *doc;
	}

	void ParserCommunicator::StoreIndex(DocumentVector index)
	{

	}

	void ParserCommunicator::StoreExpectedURIRelevance(URIPacket uriRelevance)
	{
		std::shared_ptr<URIPacket> uriPacket(new URIPacket(uriRelevance));

		ThywinPacket thywinPacket;
		thywinPacket.Method = PUT;
		thywinPacket.Type = URI;
		thywinPacket.Content = uriPacket;

		communicator.SendPacket(thywinPacket);
	}

	void ParserCommunicator::StoreActualURIRelevance(URIPacket uriRelevance)
	{
		std::shared_ptr<URIPacket> uriPacket(new URIPacket(uriRelevance));

		ThywinPacket thywinPacket;
		thywinPacket.Method = PUT;
		thywinPacket.Type = RELEVANCE;
		thywinPacket.Content = uriPacket;

		communicator.SendPacket(thywinPacket);
	}

} /* namespace thywin */
