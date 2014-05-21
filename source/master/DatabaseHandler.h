/*
 * DatabaseHandler.h
 *
 *  Created on: May 15, 2014
 *      Author: Thomas Gerritsen
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

	class DatabaseHandler
	{
		public:

			/**
			 * Create a database handler that will connect to a ODBC database.
			 * This will connect to the default ODBC port.
			 * @param ipaddress The IP of the machine where the database is running
			 */
			DatabaseHandler(std::string ipaddress);

			/**
			 * Create a database handler that will connect to a ODBC database
			 * @param ipaddress The IP of the machine where the database is running
			 * @param givenport The port on which the database is listening
			 */
			DatabaseHandler(std::string ipaddress, int givenPort);
			virtual ~DatabaseHandler();

			/**
			 * Sets up a connectino to the database of given IP.
			 * In case a port was given with the constructor it will connect to a database of that port.
			 */
			void Connect();

			/**
			 * Delete an URI from a table.
			 * @param URI String of the URI that will be deleted.
			 * @param table Name of the table from which the URI will be deleted.
			 */
			void DeleteURIFrom(std::string URI, std::string table, bool all);

			/**
			 * Adds a new URI to the URI List table. This table is a collection of all URIs found.
			 * @param element Add a new URI to the URI List table.
			 */
			void AddURIToList(std::shared_ptr<URIPacket> element);

			/**
			 * Adds a new URI to the URI Queue table.
			 * @param URI The URI that will be added to the queue
			 */
			void AddURIToQueue(std::string URI);

			/**
			 * Add a document to the Qeueu.
			 * @param input A documentPacket containing a URI & Content.
			 * URI should already be in the list.
			 */
			void AddDocumentToQueue(std::shared_ptr<DocumentPacket> input);

			/**
			 *
			 */
			void AddWordcountToIndex(std::string URI, std::string word, int count);

			/**
			 *
			 */
			std::shared_ptr<URIPacket> RetrieveURIFromQueue();

			/**
			 *
			 */
			std::shared_ptr<URIPacket> RetrieveAndDeleteURIFromQueue();

			/**
			 *
			 */
			std::shared_ptr<DocumentPacket> RetrieveDocumentFromQueue();

			/**
			 *
			 */
			std::shared_ptr<DocumentPacket> RetrieveAndDeleteDocumentFromQueue();

			/**
			 * Get a list of URIpackets from the URI Queue table in the database.
			 * @return list of URIpackets. Shared pointers.
			 */
			std::vector<std::shared_ptr<URIPacket>> GetURIListFromQueue();

			/**
			 * Get the number of rows that are within the given queue table.
			 * @param queue The name of the queue that will be checked.
			 * @return Number representing the amount of rows in the queue table.
			 */
			int GetRowCount(std::string queue);

			/**
			 * Checks whatever or not a queue table in the database is empty.
			 * @param queue The name of the queue that will be checked
			 * @return true if the queue table is empty. False if not.
			 */
			bool IsQueueEmpty(std::string queue);

			/**
			 *
			 */
			void Disconnect(SQLHANDLE& stmtHndl);

			/**
			 *
			 */
			void Disconnect();

		private:
			void handleNonRowReturningQuery(std::string query);
			bool executeQuery(std::string query, SQLHANDLE& stmtHndl);
			bool connectionCheck();
			void show_error(unsigned int handletype, const SQLHANDLE& handle);

			/**
			 * Creates a new statement handler.
			 */
			SQLHANDLE createStatementHandler();

			/**
			 * Closes a statement handler. Should always be called after every query!
			 */
			void releaseStatementHandler(SQLHANDLE& handler);
	};

} /* namespace thywin */

#endif /* DATABASEHANDLER_H_ */
