/*
 * Communicator.h
 *
 *  Created on: May 13, 2014
 *      Author: damonk
 */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include "ThywinPacket.h"

#include <string>

namespace thywin
{
	/**
	 * A special character used for serialization.
	 * SEP is used to divide between values in a @ThywinPacket.
	 * Should not be used in content!
	 */
	const char SEP = (char) 30;

	/**
	 * Special character used for serialization.
	 * EOT is used to mark the end of a ThywinPacket.
	 * Should not be used in content!
	 */
	const char EOT = (char) 4;

	/**
	 * STX is a special character used for serialization.
	 * STX Is used to divide values within the @Content of a @ThywinPacket.
	 * Expected to be used after each separate value within a struct or class, including the last value.
	 */
	const char STX = (char) 3;


	/**
	 * Communication class for connecting with a server.
	 * Handles receiving and sending of ThywinPackets as well as (de)serialization of it's contents.
	 */
	class Communicator
	{
		public:
			/**
			 * @ipaddress	std::string containing a valid IP address of a server
			 * 				 that the communicator needs to connect to
			 *
			 * 	Creates an new object with a connection set up to the given IP.
			 */
			Communicator(std::string ipaddress);
			virtual ~Communicator();

			/**
			 * @packet		ThywinPacket. Automatically serializes the TPObject content.
			 * 				 Content can be NULL when sending a GET request
			 *
			 * Send a ThywinPacket to the connected server. Handles serialization of the packet.
			 */
			int SendPacket(ThywinPacket packet);

			/**
			 * @obj			TPObject. Shared pointer to the TPObject that will contain the content
			 * 				 from the RESPONSE reply from a GET request
			 * @return		ThywinPacket. Shared pointer to a ThywinPacket,
			 * 				 containing the entire response from the connected server
			 *
			 * Needs to be called after a GET request. Handles deserialization of TPObject.
			 * Function will wait until the entire reponse from the connected server has come through.
			 */
			std::shared_ptr<ThywinPacket> ReceivePacket(std::shared_ptr<TPObject> obj);

			/**
			 * Closes the connection & socket with the server.
			 */
			void CloseConnection();

		private:
			int connectionSocket;
	};

} /* namespace thywin */

#endif /* COMMUNICATOR_H_ */
