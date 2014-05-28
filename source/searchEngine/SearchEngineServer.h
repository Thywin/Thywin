/*
 * SearchEngineServer.h
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin
 */

#ifndef SEARCHENGINESERVER_H_
#define SEARCHENGINESERVER_H_

namespace thywin
{
	/**
	 *	The number of clients that can be accepted for the server.
	 *	This is the max number of clients that can be listened to.
	 */
	const int AMOUNT_OF_CONNECTIONS_ACCEPT_DEFAULT = 127;
	
	class SearchEngineServer
	{
		public:
			/**
			 * Creates the SearchEngineServer object and sets up the server on given port.
			 * @param port Valid port number. Can be any number between 256 and 65535.
			 * @param accept Number of accepted connections at any given time.
			 */
			SearchEngineServer(const int port, const int accept);

			virtual ~SearchEngineServer();
			/**
			 * Check whatever or not the server has a connection set up.
			 * @return returns true when the server has connection (is setup) otherwise returns false.
			 */
			bool HasConnection();
		private:
			int serverSocket;
			bool connection;
			int maximumNumbersOfConnections;
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

} /* namespace thywin */

#endif /* SEARCHENGINESERVER_H_ */
