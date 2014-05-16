/*
 * communicator.cpp
 *
 *  Created on: 10 mei 2014
 *      Author: Thomas
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
#include <exception>

namespace thywin
{
	ThywinPacket MasterCommunicator::HandleGetURI()
	{
		ThywinPacket packet;
		packet.Method = RESPONSE;
		packet.Type = URI;
		packet.Content = Master::GetNextURIElementFromQueue();

		return packet;
	}

	ThywinPacket MasterCommunicator::HandleGetDocument()
	{
		ThywinPacket packet;
		packet.Method = RESPONSE;
		packet.Type = URI;
		packet.Content = Master::GetNextDocumentElementFromQueue();
		return packet;
	}

	void MasterCommunicator::HandlePutURI(std::shared_ptr<TPObject> content)
	{
		std::shared_ptr<URIPacket> packet = std::dynamic_pointer_cast<URIPacket>(content);
		Master::AddURIElementToQueue(packet);
	}

	void MasterCommunicator::HandlePutDocument(std::shared_ptr<TPObject> content)
	{
		std::shared_ptr<DocumentPacket> packet = std::dynamic_pointer_cast<DocumentPacket>(content);
		Master::AddDocumentElementToQueue(packet);
	}

}