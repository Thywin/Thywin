/*
 * URIPacket.cpp
 *
 *  Created on: 15 mei 2014
 *      Author: Thomas Kooi
 */

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "Communicator.h"
#include "URIPacket.h"

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
	}
}
