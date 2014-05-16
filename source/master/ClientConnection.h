/*
 * ClientConnection.h
 *
 *  Created on: 14 mei 2014
 *      Author: Thomas
 */

#ifndef CLIENTCONNECTION_H_
#define CLIENTCONNECTION_H_
#include "Communicator.h"
#include <sstream>

namespace thywin
{

	class ClientConnection
	{
		public:
			/**
			 * Creates a connection class for handling the connection from client to Master Server.
			 * @Client:		Socket Descriptor for the client connection. Has to be a live connection
			 *
			 */
			ClientConnection(int client);

			/**
			 * Default destructor. Closes the connection with the client if still alive
			 */
			virtual ~ClientConnection();

			/**
			 * Call function to start handling the connection; function is blocking.
			 * Function stops when the connection has been closed.
			 */
			void HandleConnection();

			/**
			 * Closes the connection with the client
			 */
			void CloseConnection();

			/**
			 * Check whatever or not there is a connection established. Does not mean the connection is being handled!
			 * @Return:		True on connection, false if connection is closed.
			 */
			bool hasConnection();

		private:
			int clientSocket;
			bool handlingConnection;
			bool connection;
			std::string lastPacketContent;

			ThywinPacket ReceivePacket();
			int SendPacket(ThywinPacket sendPacket);
			void HandleGetRequests(ThywinPacket packet);
			void HandlePutRequests(ThywinPacket packet);
	};

} /* namespace thywin */

#endif /* CLIENTCONNECTION_H_ */
