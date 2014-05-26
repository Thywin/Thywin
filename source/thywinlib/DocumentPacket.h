/*
 * Document.h
 *
 *  Created on: May 13, 2014
 *      Author: Thomas Kooi
 *	 	 	 	Imre Woudstra
 */

#ifndef DOCUMENT_H_
#define DOCUMENT_H_
#include "ThywinPacketContent.h"
#include <memory>
#include <vector>

namespace thywin
{
	/**
	 * Object to store documents and their location on the web.
	 * Has build in functionality to serialize and deserialize for
	 * sending across the network between clients and server.
	 */
	class DocumentPacket: public ThywinPacketContent
	{
		public:
			typedef std::shared_ptr<DocumentPacket> DocumentPacketPtr;
			typedef std::vector<DocumentPacketPtr> DocumentPacketVector;

			/**
			 * String containing a URI, location of the document on the web.
			 */
			std::string URI;

			/**
			 * Contains a document. Usually the entire content of a web page.
			 */
			std::string Document;

			/**
			 * Serializes the DocumentPacket to a string so it can be send over a socket
			 * @return	String version of the DocumentPacket.
			 */
			std::string Serialize();

			/**
			 * Deserializes a string to a DocumentPacket. Fills the URI and Document with the content from the string.
			 * @param input serialized DocumentPacket that will be deserialized to a DocumentPacket
			 */
			void Deserialize(const std::string& input);
	};

} /* namespace thywin */

#endif /* DOCUMENT_H_ */
