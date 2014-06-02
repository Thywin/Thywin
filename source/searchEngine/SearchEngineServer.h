/*
 * SearchEngineServer.h
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin Janssen
 *      Author: Thomas Kooi
 */

#ifndef SEARCHENGINESERVER_H_
#define SEARCHENGINESERVER_H_

#include <Logger.h>

namespace thywin
{
	class SearchEngineServer
	{
		public:
			/**
			 * Creates a SearchEngineServer object and sets up the server on the given port.
			 * @param port The port that this server will listen on. Must a be valid port number (can be any number between 256 and 65535)
			 * @param maxNumberOfConnections The maximum amount of concurrent connections.
			 */
			SearchEngineServer(const int port, const int maxNumberOfConnections);

			/**
			 * Default destructor
			 */
			virtual ~SearchEngineServer();

			
		private:
			Logger logger;
			int listenSocketFD;
			bool socketBound;
			int maxNumberOfConnections;
			
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
			
			static void* setUpConnectionWithClient(void *socket);
	};

} /* namespace thywin */

#endif /* SEARCHENGINESERVER_H_ */
