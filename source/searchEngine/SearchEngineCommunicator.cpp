/*
 * SearchEngineCommunicator.cpp
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin
 */

#include "SearchEngineCommunicator.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <string.h>
#include <string>
#include <errno.h>
#include <stdexcept>
#include "Logger.h"
#include "Communicator.h"
#include "DocumentVectorPacket.h"
#include "SearchEngine.h"

namespace thywin
{
	
	/**
	 * Loggers are temporarily placed in comments while awaiting library update
	 */
	SearchEngineCommunicator::SearchEngineCommunicator(int client)
	{
		std::stringstream out;
		out << "Master_connection_" << client << ".log";
		std::string logFileName = out.str();
		clientSocket = client;
		handlingConnection = false;
		connection = true;
	}
	
	SearchEngineCommunicator::~SearchEngineCommunicator()
	{
		handlingConnection = false;
		connection = false;
		CloseConnection();
	}
	
	void SearchEngineCommunicator::HandleGetRequest(const ThywinPacket& packet)
	{
		ThywinPacket returnPacket;
		returnPacket.Method = RESPONSE;
		
		if (packet.Type == SEARCH_RESULTS)
		{
			returnPacket.Content = getSearchResults(packet.Content);
			
			try
			{
				SendPacket(returnPacket);
			}
			catch (std::runtime_error& e)
			{
				printf("Error while sending return packet: %s\n", e.what());
			}
		}
	}
	
	std::shared_ptr<MultiURIPacket> SearchEngineCommunicator::getSearchResults(std::shared_ptr<ThywinPacketContent> content)
	{
		std::cout << content << std::endl;
		std::stringstream contentStream;
		contentStream << content;
		std::string searchWords;
		std::getline(contentStream, searchWords, TP_END_OF_PACKET);
		
		
		
		SearchEngine searchEngine;
		return std::shared_ptr<MultiURIPacket>(new MultiURIPacket(searchEngine.Search(searchWords)));
		
		
		

	}
	void SearchEngineCommunicator::HandlePutRequest(const ThywinPacket& packet)
	{
	}
	
	bool SearchEngineCommunicator::hasConnection()
	{
		return connection;
	}
	
	void SearchEngineCommunicator::CloseConnection()
	{
		if (hasConnection())
		{
			printf("Closing Connection\n");
			handlingConnection = false;
			connection = false;
			
			if (close(clientSocket) < 0)
			{
				// who cares?
			}
		}
	}
	
	void SearchEngineCommunicator::HandleConnection()
	{
		handleReceivedThywinPacket(ReceivePacket());
	}
	
	int SearchEngineCommunicator::SendPacket(ThywinPacket& packet)
	{
		std::stringstream data;
		data << packet.Method << TP_HEADER_SEPERATOR << packet.Type << TP_HEADER_SEPERATOR;
		if (packet.Content != NULL)
		{
			data << packet.Content->Serialize();
		}
		data << TP_END_OF_PACKET;
		
		int sendSize = send(clientSocket, (const char*) data.str().c_str(), data.str().size(), 0);
		if (sendSize < 0)
		{
			throw std::runtime_error(std::string(strerror(errno)));
		}
		return sendSize;
	}
	
	ThywinPacket SearchEngineCommunicator::ReceivePacket()
	{
		std::stringstream receiveBuffer;
		char characterReceiveBuffer;
		int receiveSize = -1;
		do
		{
			receiveSize = recv(clientSocket, &characterReceiveBuffer, 1, 0);
			checkReceiveSize(receiveSize);
			receiveBuffer << characterReceiveBuffer;
		} while (receiveSize > 0 && characterReceiveBuffer != TP_END_OF_PACKET);
		return createThywinPacket(receiveBuffer);
	}
	
	void SearchEngineCommunicator::fillThywinPacket(ThywinPacket& packet, std::stringstream& stream)
	{
		std::string valueForPacket;
		std::cout << stream.str() << std::endl;
		std::getline(stream, valueForPacket, TP_HEADER_SEPERATOR);
		packet.Method = (PacketMethod) std::stoi(valueForPacket);
		
		std::getline(stream, valueForPacket, TP_HEADER_SEPERATOR);
		packet.Type = (PacketType) std::stoi(valueForPacket);
		if (packet.Method == PUT)
		{
			std::getline(stream, valueForPacket, TP_HEADER_SEPERATOR);
			deserializePutObject(packet, valueForPacket);
		}
		
		//std::getline(stream, valueForPacket, TP_END_OF_PACKET);
		//packet.Content = valueForPacket;
	}
	
	void SearchEngineCommunicator::deserializePutObject(ThywinPacket& packet, std::string& serializedObject)
	{
		if (packet.Method != PUT)
		{
			throw std::invalid_argument(std::string("Called deserialize object for a non PUT packet"));
		}
	}
	
	void SearchEngineCommunicator::handleReceivedThywinPacket(const ThywinPacket& packet)
	{
		switch (packet.Method)
		{
			case GET:
				HandleGetRequest(packet);
				break;
			case PUT:
				HandlePutRequest(packet);
				break;
			case RESPONSE:
				break;
		}
	}
	
	ThywinPacket SearchEngineCommunicator::createThywinPacket(std::stringstream& receiveBuffer)
	{
		ThywinPacket returnPacket(GET, SEARCH_RESULTS);
		try
		{
			fillThywinPacket(returnPacket, receiveBuffer);
		}
		catch (std::exception& e)
		{
			printf("Couldn't fill ThywinPacket\n");
		}
		return returnPacket;
	}
	
	void SearchEngineCommunicator::checkReceiveSize(const int receiveSize)
	{
		if (receiveSize < 0)
		{
			std::string errorMessage(strerror(errno));
			CloseConnection();
			throw std::runtime_error(errorMessage);
		}
		else if (receiveSize == 0)
		{
			printf("Client Closed Connection\n");
			CloseConnection();
			throw std::runtime_error("Client Closed Connection");
		} // There are no other cases that would need to be handled
	}

} /* namespace thywin */
