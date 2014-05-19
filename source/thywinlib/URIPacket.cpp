/*
 * URIPacket.cpp
 *
 *  Created on: 15 mei 2014
 *      Author: Thomas Kooi
 */

#include <string>
#include <iostream>
#include "Communicator.h"
#include <stdlib.h>
#include "URIPacket.h"
#include <stdio.h>

namespace thywin
{
	std::string URIPacket::Serialize()
	{
		std::stringstream stream;
		stream << Relevance << TP_CONTENT_SEPERATOR << URI << TP_CONTENT_SEPERATOR;
		return stream.str();
	}

	void URIPacket::Deserialize(const std::string& input)
	{
		std::stringstream stream;
		stream << input;

		std::string extractedValue;
		std::getline(stream, extractedValue, TP_CONTENT_SEPERATOR);
		Relevance = atof(extractedValue.c_str());

		std::getline(stream, URI, TP_CONTENT_SEPERATOR);

		printf("deserialize: %s | %f\n", URI.c_str(), Relevance);
	}

}
