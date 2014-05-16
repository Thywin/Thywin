/*
 * DocumentPacket.cpp
 *
 *  Created on: 15 mei 2014
 *      Author: Thomas
 */

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
#include "DocumentPacket.h"

namespace thywin
{

	std::string DocumentPacket::Serialize()
	{
		std::stringstream stream;
		stream << URI << STX << Document << STX;
		return stream.str();
	}

	void DocumentPacket::Deserialize(const std::string& input)
	{
		std::stringstream stream;
		stream << input;

		std::string token;
		std::getline(stream, URI, STX);
		std::getline(stream, Document, STX);
	}
}
