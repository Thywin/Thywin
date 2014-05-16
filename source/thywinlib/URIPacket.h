/*
 * URIElement.h
 *
 *  Created on: May 13, 2014
 *      Author: Thomas Kooi
 *      		Imre Woudstra
 */

#ifndef URIELEMENT_H_
#define URIELEMENT_H_
#include "TPObject.h"

namespace thywin
{
	/**
	 * An URIPacket is a packet which contains a URI and the Relevance of that URI.
	 */
	class URIPacket: public TPObject
	{
		public:
			double Relevance;
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
