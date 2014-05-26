/*
 * DocumentVectorPacket.cpp
 *
 *  Created on: 23 mei 2014
 *      Author: Bobby
 */

#include "Communicator.h"
#include "DocumentVectorPacket.h"
#include "DocumentVector.h"

namespace thywin
{

	DocumentVectorPacket::DocumentVectorPacket(std::string packetURI, double packetRelevance,
			DocumentVector packetIndex)
	{
		URI = packetURI;
		Relevance = packetRelevance;
		Index = packetIndex;
	}

	DocumentVectorPacket::DocumentVectorPacket()
	{
		Relevance = 0;
	}

	std::string DocumentVectorPacket::Serialize()
	{
		std::stringstream documentVectorStream;
		documentVectorStream << URI << TP_CONTENT_SEPERATOR << Relevance << TP_CONTENT_SEPERATOR << Index.Serialize()
				<< TP_CONTENT_SEPERATOR;
		return documentVectorStream.str();
	}

	void DocumentVectorPacket::Deserialize(const std::string& input)
	{
		std::stringstream stream;
		stream << input;

		std::string serializedLine;
		std::getline(stream, URI, TP_CONTENT_SEPERATOR);
		std::getline(stream, serializedLine, TP_CONTENT_SEPERATOR);

		try
		{
			Relevance = std::stoi(serializedLine);
		}
		catch (std::exception& e)
		{
			Relevance = 0;
		}

		std::getline(stream, serializedLine, TP_END_OF_PACKET);
		DocumentVector vector;
		vector.Deserialize(serializedLine);
		Index = vector;
	}
}

