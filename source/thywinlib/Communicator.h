/*
 * Communicator.h
 *
 *  Created on: May 13, 2014
 *      Author: Thomas Kooi
 *      		Imre Woudstra
 */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include "ThywinPacket.h"

#include <string>

namespace thywin
{
	/**
	 * A special character used for serialization.
	 * TP_HEADER_SEPERATOR is used to divide between values in a @ThywinPacket.
	 * Should not be used in content!
	 */
	const char TP_HEADER_SEPERATOR = (char) 30;

	/**
	 * Special character used for serialization.
	 * TP_END_OF_PACKET is used to mark the end of a ThywinPacket.
	 * Should not be used in content!
	 */
	const char TP_END_OF_PACKET = (char) 4;

	/**
	 * TP_CONTENT_SEPERATOR is a special character used for serialization.
	 * TP_CONTENT_SEPERATOR Is used to divide values within the @Content of a @ThywinPacket.
	 * Expected to be used after each separate value within a struct or class, including the last value.
	 */
	const char TP_CONTENT_SEPERATOR = (char) 3;

	/**
	 * TP_CONTENT_VECTOR_SEPERATOR is a special character used for serialization
	 * TP_CONTENT_VECTOR_SEPERATOR Is used to divide values within the @Content of a @ThywinPacket
	 * based on a MultiURIPacket.
	 * Expected to be used after each separate value within a struct or class, including the last value.
	 */
	const char TP_CONTENT_VECTOR_SEPERATOR = (char) 2;

	/**
	 * Communication class for connecting with a server.
	 * Handles receiving and sending of ThywinPackets as well as (de)serialization of it's contents.
	 */
	class Communicator
	{
		public:
			/**
			 * Creates a new object with a connection set up to the given IP.
			 * @param ipaddress std::string containing a valid IP address of a server
			 * that the communicator needs to connect to
			 * @param serverPort port of the server that the communicator needs to connect to.
			 */
			Communicator(const std::string& ipaddress, const int& serverPort);
			virtual ~Communicator();

			/**
			 * Send a ThywinPacket to the connected server. Handles serialization of the packet.
			 * @param packet ThywinPacket. Automatically serializes the TPObject content.
			 * Content can be NULL when sending a GET request
			 */
			int SendPacket(const ThywinPacket& packet);

			/**
			 * Needs to be called after a GET request. Handles deserialization of TPObject.
			 * Function will wait until the entire reponse from the connected server has come through.
			 * @param contentObject	TPObject. Shared pointer to the TPObject that will contain the content
			 * from the RESPONSE reply from a GET request
			 * @return ThywinPacket. Shared pointer to a ThywinPacket,
			 * containing the entire response from the connected server
			 */
			std::shared_ptr<ThywinPacket> ReceivePacket(std::shared_ptr<ThywinPacketContent> contentObject);

		private:
			int connectionSocket;
			void checkRecv(int receiveSize, int socket);
	};

} /* namespace thywin */

#endif /* COMMUNICATOR_H_ */
