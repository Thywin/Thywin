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

	const int DEFAULT_DATABASE_PORT = 5432;
	const std::string DEFAULT_DATABASE_IP = "192.168.100.13";

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
			 * @param givenPort The port on which the database is listening
			 */
			DatabaseHandler(std::string ipaddress, int givenPort);
			virtual ~DatabaseHandler();

			/**
			 * Delete an URI from a table.
			 * @param URI String of the URI that will be deleted.
			 * @param table Name of the table from which the URI will be deleted.
			 */
			void DeleteURIFrom(std::string URI, std::string table);

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
			 * Add a document to the Queue.
			 * @param input A documentPacket containing a URI & Content.
			 * URI should already be in the list.
			 */
			void AddDocumentToQueue(std::shared_ptr<DocumentPacket> input);

			/**
			 * Adds the word to the database.
			 * @param URI The URI where the word came from
			 * @param word The word that is retrieved from the crawled document
			 * @param count The amount of occurrences of that word in the document
			 */
			void AddWordcountToIndex(std::string URI, std::string word, int count);

			/**
			 * Retrieves the first URI in the queue.
			 * @return Shared pointer to the URI and relevance.
			 */
			std::shared_ptr<URIPacket> RetrieveURIFromQueue();

			/**
			 * Retrieves the first URI in the queue and deletes the entry in the database.
			 * @return Shared pointer to the URI and relevance.
			 */
			std::shared_ptr<URIPacket> RetrieveAndDeleteURIFromQueue();

			/**
			 * Retrieves the first document in the queue.
			 * @return Shared pointer to the URI and content.
			 */
			std::shared_ptr<DocumentPacket> RetrieveDocumentFromQueue();

			/**
			 * Retrieves the first document in the queue and deletes the entry in the database.
			 * @return Shared pointer to the URI and content.
			 */
			std::shared_ptr<DocumentPacket> RetrieveAndDeleteDocumentFromQueue();

			/**
			 * Get a list of URIpackets from the URI Queue table in the database.
			 * @param amount The amount of URIs you want to get.
			 * @return list of URIpackets. Shared pointers.
			 */
			std::vector<std::shared_ptr<URIPacket>> GetURIListFromQueue(const int amount);

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
			 * Frees the given and global handles and disconnects the created connection.
			 * @param stmtHndl The in-scope statement handle
			 */
			void Disconnect(SQLHANDLE& stmtHndl);

			/**
			 * Frees the global handles and disconnects the created connection.
			 */
			void Disconnect();

		private:
			SQLHANDLE environmentHandle;
			SQLHANDLE connectionHandle;
			bool connected;
			static const int CONNECTION_TIMEOUT_IN_MINUTES = 15;
			static const int RETRIEVE_URI_BUFFER_SIZE = 1024;
			static const int SQL_ERRORSTATE_UNIQUE = 23505;
			static const int DEFAULT_BUFFER_SIZE = 1024;

			void handleNonRowReturningQuery(std::string query);
			bool executeQuery(std::string query, SQLHANDLE& stmtHndl);
			void showError(unsigned int handletype, const SQLHANDLE& handle);

			/**
			 * Creates a new statement handler.
			 */
			SQLHANDLE createStatementHandler();

			/**
			 * Closes a statement handler. Should always be called after every query!
			 */
			void releaseStatementHandler(SQLHANDLE& handler);

			/**
			 * Sets up a connection to the database of given IP.
			 * In case a port was given with the constructor it will connect to a database of that port.
			 */
			void Connect(std::string ipaddress, int givenPort);
	};

} /* namespace thywin */

#endif /* DATABASEHANDLER_H_ */
