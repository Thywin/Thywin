/*
 * communicator.cpp
 *
 *  Created on: 10 mei 2014
 *      Author: Thomas Kooi
 */

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <mutex>
#include <fstream>
#include "Master.h"
#include "MasterCommunicator.h"
#include "Communicator.h"
#include "URIPacket.h"
#include "DocumentPacket.h"

namespace thywin
{
	ThywinPacket MasterCommunicator::HandleGetURI()
	{
		ThywinPacket packet;
		packet.Method = RESPONSE;
		packet.Type = URI;
		packet.Content = Master::GetNextURIElementFromQueue();
		printf("Sending back a URI: %i\n",packet.Content->Serialize().size());
		return packet;
	}

	ThywinPacket MasterCommunicator::HandleGetDocument()
	{
		ThywinPacket packet;
		packet.Method = RESPONSE;
		packet.Type = URI;

		packet.Content = Master::GetNextDocumentElementFromQueue();
		printf("Sending back a document: %i [COMPLETED]\n",packet.Content->Serialize().size());
		return packet;
	}

	void MasterCommunicator::HandlePutURI(std::shared_ptr<ThywinPacketContent> content)
	{
		std::shared_ptr<URIPacket> packet = std::dynamic_pointer_cast<URIPacket>(content);
		printf("Putting URI: %s\n",packet->URI.c_str());
		Master::AddURIElementToQueue(packet);
	}

	void MasterCommunicator::HandlePutDocument(std::shared_ptr<ThywinPacketContent> content)
	{
		std::shared_ptr<DocumentPacket> packet = std::dynamic_pointer_cast<DocumentPacket>(content);
		printf("Putting document: %s\n",packet->URI.c_str());
		Master::AddDocumentElementToQueue(packet);
	}

}
