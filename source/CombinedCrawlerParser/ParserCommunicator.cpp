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

	void ParserCommunicator::StoreIndex(const DocumentVectorPacket& index)
	{
		std::shared_ptr<DocumentVectorPacket> documentVectorPtr(new DocumentVectorPacket(index));

		ThywinPacket thywinPacket(PUT, DOCUMENTVECTOR, documentVectorPtr);

		unsigned int bytesSent = communicator.SendPacket(thywinPacket);
		std::stringstream logMessageStoreIndex;
		logMessageStoreIndex << "Stored index with size: " << bytesSent;
		logger.Log(INFO, logMessageStoreIndex.str());
	}

	void ParserCommunicator::StoreMultipleURIs(const MultiURIPacket& multiURIPacket)
	{
		std::shared_ptr<MultiURIPacket> multiURIPacketSPtr(new MultiURIPacket(multiURIPacket));

		ThywinPacket packet(PUT, URIVECTOR, multiURIPacketSPtr);

		unsigned int bytesSent = communicator.SendPacket(packet);
		std::stringstream logMessageStoreMultipleURIs;
		logMessageStoreMultipleURIs << "Stored uri's with size: " << bytesSent;
		logger.Log(INFO, logMessageStoreMultipleURIs.str());
	}

} /* namespace thywin */
