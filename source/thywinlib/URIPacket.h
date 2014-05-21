/*
 * URIElement.h
 *
 *  Created on: May 13, 2014
 *      Author: Thomas Kooi
 *      		Imre Woudstra
 */

#ifndef URIELEMENT_H_
#define URIELEMENT_H_
#include "ThywinPacketContent.h"
#include <memory>
#include <vector>

namespace thywin
{
	/**
	 * An URIPacket is a packet which contains a URI and the Relevance of that URI.
	 */
	class URIPacket: public ThywinPacketContent
	{
		public:
			typedef std::shared_ptr<URIPacket> URIPacketPtr;
			typedef std::vector<std::shared_ptr<URIPacket>> URIPacketVector;

			/**
			 * double that represents the relevance of the URI  
			 */
			double Relevance;
			/**
			 * String containing a URI, location of a document on the web.
			 */
			std::string URI;

			/**
			 * Serialize the URIPacket to a string so it can be sent over a socket.
			 * @return the string that represents the serialized URIPacket
			 */
			std::string Serialize();

			/**
			 * Deserialize a string to an URIPacket
			 * @param input serialized URIPacket that will be deserialized to a URIPacket
			 */
			void Deserialize(const std::string& input);
	};
} /* namespace thywin */

#endif /* URIELEMENT_H_ */
