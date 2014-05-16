/*
 * ThywinPacket.h
 *
 *  Created on: May 13, 2014
 *      Author: Thomas Kooi
 *      		Imre Woudstra
 */

#ifndef THYWINPACKET_H_
#define THYWINPACKET_H_

#include <sstream>
#include <iostream>
#include <memory>
#include "TPObject.h"

namespace thywin
{
	/**
	 * Defines the type of Method from a message between client and server.
	 * Can be any of the above listed. RESPONSE is a reply on a GET message.
	 */
	enum PacketMethod
	{
		/** Request content of the given type @PacketType */
		GET = 1,
		/** Send a put message of the given type @PacketType */
		PUT = 2,
		/** Reply to a GET request. Expected to be of same @PacketType as the GET request */
		RESPONSE = 3
	};

	/*
	 * PacketType is the type of a ThywinPacket when send across the network between client and server.
	 * The type identifies the content send within the package.
	 */
	enum PacketType
	{
		/** Uniform resource identifier */
		URI = 1,
		/** A Document or content of a web page */
		DOCUMENT = 2,
		/** */
		RELEVANCE = 3,
		/** */
		DOCUMENTVECTOR = 4
	};

	/**
	 * Struct for sending over messages between clients and servers.
	 */
	struct ThywinPacket
	{
			/**
			 * Contains the method of the ThywinPacket. @PacketMethod
			 */
			PacketMethod Method;

			/**
			 * Contains the type of the ThywinPacket. @PacketType
			 */
			PacketType Type;

			/**
			 * Shared pointer to the content of the packet. This is the message/information that will be send across.
			 * When using the @PacketMethod GET, Content can be of value NULL.
			 */
			std::shared_ptr<TPObject> Content;
	};

} /* namespace thywin */

#endif /* THYWINPACKET_H_ */
