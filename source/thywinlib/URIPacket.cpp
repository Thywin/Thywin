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

namespace thywin
{
	std::string URIPacket::Serialize()
	{
		std::stringstream stream;
		stream << Relevance << STX << URI << STX;
		return stream.str();
	}

	void URIPacket::Deserialize(std::string input)
	{
		std::stringstream stream;
		stream << input;

		std::string token;
		std::getline(stream, token, STX);
		Relevance = atoi(token.c_str());

		std::getline(stream, URI, STX);
	}

}