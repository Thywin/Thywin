/*
 * SearchEngineCommunicator.h
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin Janssen
 */

#ifndef SEARCHENGINECOMMUNICATOR_H_
#define SEARCHENGINECOMMUNICATOR_H_

#include <string>

#include <Logger.h>
#include <MultiURIPacket.h>

namespace thywin
{
	class SearchEngineCommunicator
	{
		public:
			/**
			 * Creates a connection object for handling a connection with the search engine.
			 * @param Client Socket Descriptor for the client connection. Has to be a live connection.
			 */
			SearchEngineCommunicator(int clientCommunicationSocket);

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



		private:
			Logger logger;
			int clientCommunicationSocket;
			bool connection;

			std::string receivePacket();
			void sendPacket(std::string packet);
			std::string getSearchResults(std::string searchWords);
	};

} /* namespace thywin */

#endif /* SEARCHENGINECOMMUNICATOR_H_ */
