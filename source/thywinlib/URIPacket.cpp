/*
 * URIPacket.cpp
 *
 *  Created on: 15 mei 2014
 *      Author: Thomas
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
		stream << Relevance << STX << URI << STX;
		return stream.str();
	}

	void URIPacket::Deserialize(const std::string& input)
	{
		std::stringstream stream;
		stream << input;

		std::string token;
		std::getline(stream, token, STX);
		Relevance = atof(token.c_str());

		std::getline(stream, URI, STX);

		printf("deserialize: %s | %f\n", URI.c_str(), Relevance);
	}

}
