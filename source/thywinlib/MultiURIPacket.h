/*
 * MultiURIPacket.h
 *
 *  Created on: 26 mei 2014
 *      Author: Bobby
 */

#ifndef MULTIURIPACKET_H_
#define MULTIURIPACKET_H_

#include <memory>
#include <string>
#include <vector>
#include "ThywinPacketContent.h"
#include "URIPacket.h"

namespace thywin
{
	/**
	 *
	 */
	class MultiURIPacket: public ThywinPacketContent
	{
		public:
			/**
			 *
			 */
			std::vector<std::shared_ptr<URIPacket>> Content;

			/**
			 * Serializes the DocumentPacket to a string so it can be send over a socket
			 * @return	String version of the DocumentPacket.
			 */
			std::string Serialize();

			/**
			 * Deserializes a string to a MultiURIPacket. Fills the URI and Document with the content from the string.
			 *
			 */
			void Deserialize(const std::string& input);
	};

} /* namespace thywin */

#endif /* MULTIURIPACKET_H_ */
