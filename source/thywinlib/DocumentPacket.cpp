/*
 * DocumentPacket.cpp
 *
 *  Created on: 15 mei 2014
 *      Author: Thomas Kooi
 */

#include <string>
#include <iostream>
#include "Communicator.h"
#include <stdlib.h>
#include "DocumentPacket.h"
#include <stdexcept>

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
		if (input.empty()) {
			throw std::invalid_argument("Input argument is empty");
		}
		std::stringstream stream;
		stream << input;

		std::string token;
		std::getline(stream, URI, TP_CONTENT_SEPERATOR);
		std::getline(stream, Document, TP_CONTENT_SEPERATOR);
	}
}
