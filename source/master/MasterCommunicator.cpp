/*
 * communicator.cpp
 *
 *  Created on: 10 mei 2014
 *      Author: Thomas Kooi
 */

#include "Master.h"
#include "MasterCommunicator.h"
#include "URIPacket.h"
#include "DocumentPacket.h"
#include "MultiURIPacket.h"
#include "DocumentVectorPacket.h"

namespace thywin
{
	ThywinPacket MasterCommunicator::HandleGetURI()
	{
		return createResponsePacket(Master::GetNextURIElementFromQueue());
	}

	ThywinPacket MasterCommunicator::HandleGetDocument()
	{
		return createResponsePacket(Master::GetNextDocumentElementFromQueue());
	}

	void MasterCommunicator::HandlePutURI(std::shared_ptr<ThywinPacketContent> content)
	{
		std::shared_ptr<URIPacket> packet = std::dynamic_pointer_cast<URIPacket>(content);
		Master::AddURIElementToQueue(packet);
	}

	void MasterCommunicator::HandlePutDocument(std::shared_ptr<ThywinPacketContent> content)
	{
		std::shared_ptr<DocumentPacket> packet = std::dynamic_pointer_cast<DocumentPacket>(content);
		Master::AddDocumentElementToQueue(packet);
	}


	void MasterCommunicator::HandlePutUriVector(std::shared_ptr<ThywinPacketContent> content)
	{
		std::shared_ptr<MultiURIPacket> packet = std::dynamic_pointer_cast<MultiURIPacket>(content);
		Master::AddMultipleURISToQueue(packet);
	}

	void MasterCommunicator::HandlePutDocumentVector(std::shared_ptr<ThywinPacketContent> content)
	{
		std::shared_ptr<DocumentVectorPacket> packet = std::dynamic_pointer_cast<DocumentVectorPacket>(content);
		Master::PutDocumentVector(packet);
	}

	ThywinPacket MasterCommunicator::createResponsePacket(std::shared_ptr<ThywinPacketContent> content)
	{
		ThywinPacket packet;
		packet.Method = RESPONSE;
		packet.Type = URI;
		packet.Content = content;
		return packet;
	}

}
