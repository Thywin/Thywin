/*
 * SearchEngineCommunicator.h
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin
 */

#ifndef SEARCHENGINECOMMUNICATOR_H_
#define SEARCHENGINECOMMUNICATOR_H_

#include "Communicator.h"
#include <sstream>
#include <memory>
#include "Logger.h"
#include "MultiURIPacket.h"

namespace thywin
{

	class SearchEngineCommunicator
	{
		public:
			/**
			 * Creates a connection class for handling the connection from client to Master Server.
			 * @param Client Socket Descriptor for the client connection. Has to be a live connection
			 */
			SearchEngineCommunicator(int client);

			/**
			 * Default destructor. Closes the connection with the client if still alive
			 */
			virtual ~SearchEngineCommunicator();

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
			int clientSocket;
			bool handlingConnection;
			bool connection;

			std::string receivePacket();
			int sendPacket(std::string packet);
			std::string getSearchResults(std::string searchWords);
			void checkReceiveSize(const int receiveSize);
	};

} /* namespace thywin */

#endif /* SEARCHENGINECOMMUNICATOR_H_ */
