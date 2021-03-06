/*
 * ClientConnection.h
 *
 *  Created on: 14 mei 2014
 *      Author: Thomas Kooi
 *      Author: Thomas Gerritsen
 */

#ifndef CLIENTCONNECTION_H_
#define CLIENTCONNECTION_H_
#include "Communicator.h"
#include "MasterCommunicator.h"
#include <sstream>
#include "Logger.h"

namespace thywin
{
	/**
	 * Handles the connection between a client and the master.
	 * For each thread with a client there will be a client connection.
	 */
	class ClientConnection
	{
		public:
			/**
			 * Creates a connection class for handling the connection from client to Master Server.
			 * @param Client Socket Descriptor for the client connection. Has to be a live connection
			 */
			ClientConnection(int client, Logger& threadLogger);

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
			 * Closes the connection & socket with the client..
			 */
			void CloseConnection();

			/**
			 * Check whatever or not there is a connection established. Does not mean the connection is being handled!
			 * @Return True on connection, false if connection is closed.
			 */
			bool hasConnection();

		private:
			Logger logger;
			int clientSocket;
			bool handlingConnection;
			std::string lastPacketContent;
			MasterCommunicator communicator;
			//Logger logger; /* Temporarily placed in comment while awaiting library update */

			ThywinPacket ReceivePacket();
			void SendPacket(ThywinPacket& sendPacket);
			void HandleGetRequest(const ThywinPacket& packet);
			void HandlePutRequest(const ThywinPacket& packet);
			void deserializePutObject(ThywinPacket& packet, std::string& serializedObject);
			void fillThywinPacket(ThywinPacket& packet, std::stringstream& buffer);
			void handleReceivedThywinPacket(const ThywinPacket& packet);
			void checkReceiveSize(const int receiveSize);
			ThywinPacket createThywinPacket(std::stringstream& receiveBuffer);
	};

} /* namespace thywin */

#endif /* CLIENTCONNECTION_H_ */
