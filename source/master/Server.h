/*
 * Server.h
 *
 *  Created on: 13 mei 2014
 *      Author: Thomas
 */

#ifndef SERVER_H_
#define SERVER_H_
#include "Communicator.h"

namespace thywin
{
	class Server
	{
		public:
			/**
			 * Creates the server object
			 */
			Server();
			virtual ~Server();

			/**
			 * Set up the server on port, if the server has not been set up yet
			 * @port:		Valid port number. Can be any number between # and #.
			 */
			void SetUp(int port);

			/**
			 * Start to listen on port from SetUp. Function is blocking.
			 * Creates a new thread for every client connected to server.
			 */
			void Listen();

		private:
			int serverSocket;
	};
}

#endif /* SERVER_H_ */
