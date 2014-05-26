/*
 * DocumentPacket.cpp
 *
 *  Created on: 15 mei 2014
 *      Author: Thomas Kooi
 */

#include <stdlib.h>
#include <string>
#include <iostream>
#include "Communicator.h"
#include "DocumentPacket.h"

namespace thywin
{

	std::string DocumentPacket::Serialize()
	{
		std::stringstream stream;
		stream << URI << TP_CONTENT_SEPERATOR << Document << TP_CONTENT_SEPERATOR;
		return stream.str();
	}

	void DocumentPacket::Deserialize(const std::string& input)
	{
		std::stringstream stream;
		stream << input;

		std::string token;
		std::getline(stream, URI, TP_CONTENT_SEPERATOR);
		std::getline(stream, Document, TP_CONTENT_SEPERATOR);
	}
}
