/*
 * MultiURIPacket.cpp
 *
 *  Created on: 26 mei 2014
 *      Author: Bobby Bouwmann
 */

#include <string>
#include <iostream>
#include <stdlib.h>
#include "Communicator.h"
#include "MultiURIPacket.h"

namespace thywin
{
	
	std::string MultiURIPacket::Serialize()
	{
		std::stringstream stream;

		for (unsigned int i = 0; i < Content.size(); i++)
		{
			stream << Content.at(i)->Serialize() << TP_CONTENT_VECTOR_SEPERATOR;
		}

		return stream.str();
	}

	void MultiURIPacket::Deserialize(const std::string& input)
	{
		std::stringstream stream;
		stream << input;

		std::string SerializedURIPacket;

		while (std::getline(stream, SerializedURIPacket, TP_CONTENT_VECTOR_SEPERATOR) > 0)
		{
			std::shared_ptr<URIPacket> packet(new URIPacket);
			packet->Deserialize(SerializedURIPacket);
			Content.insert(Content.end(), packet);
		}
	}

} /* namespace thywin */
