/*
 * DatabaseHandler.cpp
 *
 *  Created on: May 15, 2014
 *      Author: Thomas Gerritsen
 *      Author: Thomas Kooi
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include "DatabaseHandler.h"
#include "Master.h"

namespace thywin
{
	DatabaseHandler::DatabaseHandler(std::string ipaddress)
	{
		Connect(ipaddress, DEFAULT_DATABASE_PORT);
	}

	DatabaseHandler::DatabaseHandler(std::string ipaddress, int givenPort)
	{
		Connect(ipaddress, givenPort);
	}

	DatabaseHandler::~DatabaseHandler()
	{
		Disconnect();
	}

	void DatabaseHandler::Connect(std::string ipaddress, int givenPort)
	{
		connected = true;
		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &environmentHandle))
		{
			Disconnect();
			throw std::runtime_error(std::string("Couldn't allocate environment handle."));
		}

		if (SQL_SUCCESS != SQLSetEnvAttr(environmentHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0))
		{
			Disconnect();
			throw std::runtime_error(std::string("Couldn't set ODBC version."));
		}

		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, environmentHandle, &connectionHandle))
		{
			Disconnect();
			throw std::runtime_error(std::string("Couldn't allocate connection handle."));
		}

		SQLSetConnectOption(connectionHandle, SQL_LOGIN_TIMEOUT, CONNECTION_TIMEOUT_IN_MINUTES);

		std::ostringstream ossPort;
		ossPort << givenPort;
		std::string temp = "DRIVER={/usr/lib/arm-linux-gnueabihf/odbc/psqlodbca.so};SERVER=" + ipaddress + ";PORT="
				+ ossPort.str() + ";DATABASE=thywin;UID=thywin;PWD=hanicampna;";
		const char* connectionString = temp.c_str();

		SQLCHAR retconstring[1024];
		SQLRETURN connect = SQLDriverConnect(connectionHandle, NULL, (SQLCHAR*) connectionString, SQL_NTS, retconstring,
				1024, NULL, SQL_DRIVER_NOPROMPT);

		switch (connect)
		{
			case SQL_SUCCESS_WITH_INFO:
			{
				showError(SQL_HANDLE_DBC, connectionHandle);
				break;
			}
			case SQL_INVALID_HANDLE:
			{
				showError(SQL_HANDLE_DBC, connectionHandle);
				Disconnect();
				throw std::runtime_error(std::string("Failed to connect to the database."));
				break;
			}
			case SQL_ERROR:
			{
				showError(SQL_HANDLE_DBC, connectionHandle);
				Disconnect();
				throw std::runtime_error(std::string("Failed to connect to the database."));
				break;
			}
			default:
			{
				break;
			}
		}
	}

	void DatabaseHandler::DeleteURIFrom(std::string URI, std::string table)
	{
		std::string query = "DELETE FROM " + table + " WHERE uri_id = (SELECT uri_id FROM uris WHERE uri = '" + URI
				+ "')";
		handleNonRowReturningQuery(query);
	}

	void DatabaseHandler::AddURIToList(std::shared_ptr<URIPacket> element)
	{
		std::ostringstream ossRelevance;
		ossRelevance << element->Relevance;
		std::string query = "INSERT INTO uris (uri, relevance) VALUES ('" + element->URI + "'," + ossRelevance.str()
				+ ")";
		handleNonRowReturningQuery(query);
	}

	void DatabaseHandler::AddURIToQueue(std::string URI)
	{
		std::string query = "INSERT INTO uri_queue VALUES ((SELECT uri_id FROM uris WHERE uri = '" + URI
				+ "'), (SELECT relevance FROM uris WHERE uri = '" + URI + "'))";
		handleNonRowReturningQuery(query);
	}

	void DatabaseHandler::AddDocumentToQueue(std::shared_ptr<DocumentPacket> input)
	{
		SQLHANDLE statementHandle = createStatementHandler();
		char* statement = (char*) "INSERT INTO document_queue VALUES ((SELECT uri_id FROM uris WHERE uri = ?), ?)";
		if (SQL_SUCCESS != SQLPrepare(statementHandle, (SQLCHAR *) statement, strlen(statement)))
		{
			showError(SQL_HANDLE_STMT, statementHandle);
			Disconnect(statementHandle);
			throw std::runtime_error(std::string("Couldn't prepare sql statement."));
		}

		SQLLEN nts = SQL_NTS;
		// Bind input->URI to the first column.
		SQLBindParameter(statementHandle, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, input->URI.size(), 0,
				(char *) input->URI.c_str(), input->URI.size(), &nts);
		// Bind input->Document to the second column.
		SQLBindParameter(statementHandle, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, input->Document.size(), 0,
				(char *) input->Document.c_str(), input->Document.size(), &nts);

		if (SQL_SUCCESS != SQLExecute(statementHandle))
		{
			showError(SQL_HANDLE_STMT, statementHandle);
		}
		releaseStatementHandler(statementHandle);
	}

	void DatabaseHandler::AddWordcountToIndex(std::string URI, std::string word, int count)
	{
		std::ostringstream ossCount;
		ossCount << count;
		std::string query = "INSERT INTO indices VALUES ((SELECT uri_id FROM uris WHERE uri = '" + URI + "'), '" + word
				+ "'," + ossCount.str() + ")";
		handleNonRowReturningQuery(query);
	}

	std::shared_ptr<URIPacket> DatabaseHandler::RetrieveURIFromQueue()
	{
		SQLHANDLE statementHandle = createStatementHandler();
		std::shared_ptr<URIPacket> result(new URIPacket);
		std::string query = "SELECT uris.uri, uri_queue.priority FROM uris, uri_queue"
				" WHERE uris.uri_id = uri_queue.uri_id ORDER BY row_number() OVER () LIMIT 1";

		if (executeQuery(query, statementHandle))
		{
			char uri[RETRIEVE_URI_BUFFER_SIZE];
			if (SQLFetch(statementHandle) == SQL_SUCCESS)
			{
				// Get data from first column
				SQLGetData(statementHandle, 1, SQL_C_CHAR, uri, RETRIEVE_URI_BUFFER_SIZE, NULL);
				result->URI = std::string(uri);
				// Get data from second column
				SQLGetData(statementHandle, 2, SQL_C_DOUBLE, &result->Relevance, 0, NULL);
			}
		}
		releaseStatementHandler(statementHandle);
		return result;
	}

	std::shared_ptr<URIPacket> DatabaseHandler::RetrieveAndDeleteURIFromQueue()
	{
		std::shared_ptr<URIPacket> result(new URIPacket);
		result = RetrieveURIFromQueue();
		if (!(result->URI.empty()))
		{
			DeleteURIFrom(result->URI, "uri_queue");
		}
		return result;
	}

	std::vector<std::shared_ptr<URIPacket>> DatabaseHandler::GetURIListFromQueue(const int amount)
	{
		std::vector<std::shared_ptr<URIPacket>> CachedURIQueue;
		SQLHANDLE statementHandle = createStatementHandler();

		std::ostringstream ossAmount;
		ossAmount << amount;
		std::string query = "SELECT uris.uri, uri_queue.priority FROM uris, uri_queue"
				" WHERE uris.uri_id = uri_queue.uri_id ORDER BY row_number() OVER () LIMIT " + ossAmount.str();

		if (executeQuery(query, statementHandle))
		{
			char uri[RETRIEVE_URI_BUFFER_SIZE];
			double priority;
			while (SQLFetch(statementHandle) == SQL_SUCCESS)
			{
				std::shared_ptr<URIPacket> result(new URIPacket);
				// First column
				SQLGetData(statementHandle, 1, SQL_C_CHAR, uri, RETRIEVE_URI_BUFFER_SIZE, NULL);
				// Second column
				SQLGetData(statementHandle, 2, SQL_C_DOUBLE, &priority, 0, NULL);
				result->URI = std::string(uri);
				result->Relevance = priority;
				CachedURIQueue.insert(CachedURIQueue.end(), result);
			}
		}
		releaseStatementHandler(statementHandle);
		query = "DELETE FROM uri_queue WHERE uri_id IN (SELECT uri_id "
				"FROM uri_queue ORDER BY row_number() OVER () LIMIT " + ossAmount.str() + ")";
		handleNonRowReturningQuery(query);

		return CachedURIQueue;
	}

	std::shared_ptr<DocumentPacket> DatabaseHandler::RetrieveDocumentFromQueue()
	{
		SQLHANDLE statementHandler = createStatementHandler();
		std::shared_ptr<DocumentPacket> result(new DocumentPacket);
		std::string query = "SELECT uris.uri, document_queue.content FROM uris, "
				"document_queue WHERE uris.uri_id = document_queue.uri_id ORDER BY row_number() OVER () LIMIT 1";

		if (executeQuery(query, statementHandler))
		{
			if (SQLFetch(statementHandler) == SQL_SUCCESS)
			{
				SQLRETURN retcode = 0;
				char uri[RETRIEVE_URI_BUFFER_SIZE];
				// Get data of first column
				if (SQLGetData(statementHandler, 1, SQL_C_CHAR, uri, RETRIEVE_URI_BUFFER_SIZE, NULL) == SQL_SUCCESS)
				{
					result->URI = std::string(uri);
					char buffer[2]; // Need room for zero terminate.
					do
					{
						// Get data of second column
						retcode = SQLGetData(statementHandler, 2, SQL_C_CHAR, &buffer, 2, NULL);
						result->Document.push_back(buffer[0]);
					} while (retcode != SQL_SUCCESS);
					result->Document.push_back('\0');
				}
			}
		}
		releaseStatementHandler(statementHandler);
		return result;
	}

	std::shared_ptr<DocumentPacket> DatabaseHandler::RetrieveAndDeleteDocumentFromQueue()
	{
		std::shared_ptr<DocumentPacket> result = RetrieveDocumentFromQueue();
		if (!result->URI.empty())
		{
			DeleteURIFrom(result->URI, "document_queue");
		}
		return result;
	}

	int DatabaseHandler::GetRowCount(std::string queue)
	{
		SQLHANDLE stmtHndl = createStatementHandler();
		int rowsInQueue = 0;
		std::string query = "SELECT count(uri_id) FROM " + queue;
		if (executeQuery(query, stmtHndl))
		{
			if (SQLFetch(stmtHndl) == SQL_SUCCESS)
			{

				SQLGetData(stmtHndl, 1, SQL_C_LONG, &rowsInQueue, 0, NULL);
			}
		}
		return rowsInQueue;
	}

	bool DatabaseHandler::IsQueueEmpty(std::string queue)
	{
		SQLHANDLE stmtHndl = createStatementHandler();
		std::string query = "SELECT uri_id FROM " + queue + " LIMIT 1";
		if (executeQuery(query, stmtHndl))
		{
			if (SQLFetch(stmtHndl) == SQL_SUCCESS)
			{
				return false;
			}
		}
		return true;
	}

	void DatabaseHandler::Disconnect(SQLHANDLE& stmtHndl)
	{
		if (connected)
		{
			SQLFreeHandle(SQL_HANDLE_STMT, stmtHndl);
			Disconnect();
		}
	}

	void DatabaseHandler::Disconnect()
	{
		if (connected)
		{
			SQLDisconnect(connectionHandle);
			SQLFreeHandle(SQL_HANDLE_DBC, connectionHandle);
			SQLFreeHandle(SQL_HANDLE_ENV, environmentHandle);
			connected = false;
		}
	}

	void DatabaseHandler::handleNonRowReturningQuery(std::string SQLQuery)
	{
		SQLHANDLE statementHandler = createStatementHandler();
		const char* query = SQLQuery.c_str();
		if (SQL_SUCCESS != SQLExecDirect(statementHandler, (SQLCHAR*) query, SQL_NTS))
		{
			showError(SQL_HANDLE_STMT, statementHandler);
		}
		releaseStatementHandler(statementHandler);
	}

	bool DatabaseHandler::executeQuery(std::string SQLQuery, SQLHANDLE& statementHandle)
	{
		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, connectionHandle, &statementHandle))
		{
			showError(SQL_HANDLE_STMT, statementHandle);
			Disconnect(statementHandle);
			throw std::runtime_error(std::string("Couldn't allocate statement handle"));
		}
		const char* query = SQLQuery.c_str();
		if (SQL_SUCCESS != SQLExecDirect(statementHandle, (SQLCHAR*) query, SQL_NTS))
		{
			showError(SQL_HANDLE_STMT, statementHandle);
			return false;
		}
		return true;
	}

	void DatabaseHandler::showError(unsigned int handletype, const SQLHANDLE& statementHandler)
	{
		SQLCHAR sqlstate[DEFAULT_BUFFER_SIZE];
		SQLCHAR message[DEFAULT_BUFFER_SIZE];
		if (SQL_SUCCESS
				== SQLGetDiagRec(handletype, statementHandler, 1, sqlstate, NULL, message, DEFAULT_BUFFER_SIZE, NULL))
		{
			std::string sqlStateString((char *) sqlstate);
			std::string::size_type statePos = sqlStateString.find(std::string(SQL_ERRORSTATE_UNIQUE));
			if (statePos == std::string::npos)
			{
				// Once the library has been updated, this will be turned into a logger message.
				std::cout << "Message: " << message << " SQLSTATE: " << sqlstate << std::endl;
			}
		}
	}

	SQLHANDLE DatabaseHandler::createStatementHandler()
	{
		SQLHANDLE stmtHndl;
		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, connectionHandle, &stmtHndl))
		{
			showError(SQL_HANDLE_STMT, stmtHndl);
			Disconnect(stmtHndl);
			throw std::runtime_error(std::string("Couldn't allocate statement handle"));
		}
		return stmtHndl;
	}

	void DatabaseHandler::releaseStatementHandler(SQLHANDLE& handler)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, handler);
	}
}
