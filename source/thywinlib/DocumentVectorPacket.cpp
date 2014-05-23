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
	std::string DocumentVectorPacket::Serialize()
	{
		std::stringstream documentVectorStream;
		documentVectorStream << URI << TP_CONTENT_SEPERATOR << Relevance << TP_CONTENT_SEPERATOR << Index.Serialize() << TP_CONTENT_SEPERATOR;
		return documentVectorStream.str();
	}

	void DocumentVectorPacket::Deserialize(const std::string& input)
	{
		std::stringstream stream;
		stream << input;

		std::string token;
		std::getline(stream, URI, TP_CONTENT_SEPERATOR);
		std::getline(stream, token, TP_CONTENT_SEPERATOR);

		try
		{
			Relevance = std::stoi(token);
		}
		catch (std::exception& e)
		{
			Relevance = 0;
		}

		std::getline(stream, token, TP_CONTENT_SEPERATOR);
		DocumentVector vector;
		vector.Deserialize(token);
		Index = vector;
	}
}

