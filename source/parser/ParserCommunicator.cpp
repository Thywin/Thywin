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

	ParserCommunicator::ParserCommunicator(const std::string& masterIP, const unsigned short masterPort) :
			logger("parser.log"), communicator(masterIP, masterPort)
	{
		logger.Log(INFO, std::string("Connected to the master, will now start parsing!"));
	}

	ParserCommunicator::~ParserCommunicator()
	{
	}

	DocumentPacket ParserCommunicator::GetDocumentFromQueue()
	{
		std::shared_ptr<DocumentPacket> documentPacketSPtr(new DocumentPacket);

		ThywinPacket requestPacket(GET, DOCUMENT);

		communicator.SendPacket(requestPacket);
		communicator.ReceivePacket(documentPacketSPtr);

		return *documentPacketSPtr;
	}

	void ParserCommunicator::StoreIndex(const DocumentVector& index)
	{

	}

	void ParserCommunicator::StoreExpectedURIRelevance(const URIPacket& uriPacket)
	{
		std::shared_ptr<URIPacket> uriPacketSPtr(new URIPacket(uriPacket));

		ThywinPacket thywinPacket(PUT, URI, uriPacketSPtr);

		communicator.SendPacket(thywinPacket);
	}

	void ParserCommunicator::StoreActualURIRelevance(const URIPacket& uriPacket)
	{
		std::shared_ptr<URIPacket> uriPacketSPtr(new URIPacket(uriPacket));

		ThywinPacket thywinPacket(PUT, RELEVANCE, uriPacketSPtr);

		communicator.SendPacket(thywinPacket);
	}

} /* namespace thywin */
