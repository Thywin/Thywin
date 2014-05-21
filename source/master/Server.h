/*
 * Server.h
 *
 *  Created on: 13 mei 2014
 *      Author: Thomas Kooi
 *
 *  Wrapper around a multi-threaded C server implementation.
 */

#ifndef SERVER_H_
#define SERVER_H_
#include "Communicator.h"
#include <vector>

namespace thywin
{
	class Server
	{
		public:

			/**
			 * Creates the server object and set up the server on the default server port.
			 */
			Server();

			/**
			 * Creates the server object and sets up the server on given port
			 * @param port Valid port number. Can be any number between # and #.
			 */
			Server(const int port);
			virtual ~Server();

			/**
			 *
			 * @return returns true when the server has connection (is setup) otherwise returns false
			 */
			bool HasConnection();

		private:
			int serverSocket;
			bool connection;
			int AMOUNT_OF_CONNECTIONS_ACCEPT;

			/**
			 * Set up the server on port, if the server has not been set up yet
			 * @param port Valid port number. Can be any number between # and #.
			 */
			void SetUp(const int port);

			/**
			 * Start to listen on port from SetUp. Function is blocking.
			 * Creates a new thread for every client connected to server
			 * that will be handled through a ClientConnection object.
			 */
			void Listen();
	};
}

#endif /* SERVER_H_ */
