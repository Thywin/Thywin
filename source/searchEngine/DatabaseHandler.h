/*
 * DatabaseHandler.h
 *
 *  Created on: May 15, 2014
 *      Author: Thomas Gerritsen
 *      Author: Thomas Kooi
 */

#ifndef DATABASEHANDLER_H_
#define DATABASEHANDLER_H_

#include <string>
#include <sqltypes.h>
#include <vector>
#include <memory>
#include "URIPacket.h"
#include "DocumentPacket.h"

namespace thywin
{
	const std::string SQL_ERRORSTATE_UNIQUE = "23505";
	const int CONNECTION_TIMEOUT_IN_MINUTES = 15;
	const int RETRIEVE_URI_BUFFER_SIZE = 1024;
	const int DEFAULT_BUFFER_SIZE = 1024;
	
	class DatabaseHandler
	{
		public:
			
			/**
			 * Create a database handler that will connect to a ODBC database
			 */
			DatabaseHandler();

			/**
			 * Default destructor
			 */
			virtual ~DatabaseHandler();

			/**
			 * Frees the given and global handles and disconnects the created connection.
			 * @param stmtHndl The in-scope statement handle
			 */
			void Disconnect(SQLHANDLE& stmtHndl);

			/**
			 * Frees the global handles and disconnects the created connection.
			 */
			void Disconnect();

			/**
			 * Executes a given SQL query
			 * @param query The SQL query to be executed
			 * @param stmtHndl The handle to the database connection
			 */
			bool executeQuery(std::string query, SQLHANDLE& stmtHndl);

			/**
			 * Creates a new statement handler.
			 */
			SQLHANDLE createStatementHandler();

			/**
			 * Closes a statement handler. Should always be called after every query!
			 */
			void releaseStatementHandler(SQLHANDLE& handler);

		private:
			SQLHANDLE environmentHandle;
			SQLHANDLE connectionHandle;
			bool connected;

			void showError(unsigned int handletype, const SQLHANDLE& handle);

			/**
			 * Sets up a connection to the database of given IP.
			 * In case a port was given with the constructor it will connect to a database of that port.
			 */
			void connect();
	};

} /* namespace thywin */

#endif /* DATABASEHANDLER_H_ */
