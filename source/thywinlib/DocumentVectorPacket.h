/*
 * DocumentVectorPacket.h
 *
 *  Created on: 23 mei 2014
 *      Author: Bobby
 */

#ifndef DOCUMENTVECTORPACKET_H_
#define DOCUMENTVECTORPACKET_H_

#include <memory>
#include <vector>
#include "ThywinPacketContent.h"
#include "DocumentVector.h"

namespace thywin
{

	class DocumentVectorPacket: public ThywinPacketContent
		{
			public:

				/**
				 * String containing a URI, location of a document on the web.
				 */
				std::string URI;

				/**
				 * double that represents the relevance of the URI.
				 */
				double Relevance;

				/**
				 * String that represents a document vector.
				 */
				DocumentVector Index;

				/**
				 * Serialize the DocumentVectorPacket to a string so it can be sent over a socket.
				 * @return the string that represents the serialized DocumentVectorPacket
				 */
				std::string Serialize();

				/**
				 * Deserialize a string to an DocumentVectorPacket
				 * @param input serialized DocumentVectorPacket that will be deserialized to a DocumentVectorPacket
				 */
				void Deserialize(const std::string& input);
		};
}

#endif /* DOCUMENTVECTORPACKET_H_ */
