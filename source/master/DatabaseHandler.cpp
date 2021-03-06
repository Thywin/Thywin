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
#include "IniParser/INIReader.h"

namespace thywin
{

	DatabaseHandler::DatabaseHandler()
	{
		connect();
	}

	DatabaseHandler::~DatabaseHandler()
	{
		Disconnect();
	}

	void DatabaseHandler::connect()
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

		INIReader reader("config.ini");
		if (reader.ParseError() < 0)
		{
			throw std::runtime_error(std::string("Couldn't find \"config.ini\""));
		}

		std::string ipaddress = reader.Get("config", "ip", "UNKOWN");
		int port = reader.GetInteger("config", "port", 0);
		std::string database = reader.Get("config", "database", "UNKOWN");
		std::string username = reader.Get("config", "username", "UNKOWN");
		std::string password = reader.Get("config", "password", "UNKOWN");
		if (ipaddress == "UNKOWN" || port == 0 || database == "UNKOWN" || username == "UNKOWN" || password == "UNKOWN")
		{
			throw std::runtime_error(
					std::string("Config file not correct. Use the config.ini.example to see how it should be used."));
		}

		std::ostringstream ossPort;
		ossPort << port;
		std::string temp = "DRIVER={/usr/lib/arm-linux-gnueabihf/odbc/psqlodbca.so};SERVER=" + ipaddress + ";PORT="
				+ ossPort.str() + ";DATABASE=" + database + ";UID=" + username + ";PWD=" + password + ";";

		SQLCHAR retconstring[1024];
		SQLRETURN connect = SQLDriverConnect(connectionHandle, NULL, (SQLCHAR*) temp.c_str(), SQL_NTS, retconstring,
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

	void DatabaseHandler::AddURIToList(URIPacket::URIPacketPtr element)
	{
		if (!element->URI.empty())
		{
			std::ostringstream ossRelevance;
			ossRelevance << element->Relevance;
			std::string query = "SELECT add_uri('" + element->URI + "'," + ossRelevance.str() + ");";
			handleNonRowReturningQuery(query);
		}
	}

	bool DatabaseHandler::URIInList(std::string URI)
	{
		SQLHANDLE statementHandler = createStatementHandler();
		std::string query = "SELECT * FROM uris WHERE uri = '" + URI + "'";

		if (executeQuery(query, statementHandler))
		{
			if (SQLFetch(statementHandler) == SQL_SUCCESS)
			{
				releaseStatementHandler(statementHandler);
				return true;
			}
		}
		releaseStatementHandler(statementHandler);
		return false;
	}

	void DatabaseHandler::AddURIToQueue(std::string URI)
	{
		if (!URI.empty())
		{
			std::string query = "INSERT INTO uri_queue VALUES ((SELECT uri_id FROM uris WHERE uri = '" + URI
					+ "'), (SELECT source_relevance FROM uris WHERE uri = '" + URI + "'))";
			handleNonRowReturningQuery(query);
		}
	}

	void DatabaseHandler::AddDocumentToQueue(DocumentPacket::DocumentPacketPtr input)
	{
		SQLHANDLE statementHandle = createStatementHandler();
		std::string statement = "INSERT INTO document_queue VALUES ((SELECT uri_id FROM uris WHERE uri = ?), ?)";
		if (SQL_SUCCESS != SQLPrepare(statementHandle, (SQLCHAR *) statement.c_str(), statement.size()))
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

	void DatabaseHandler::AddIndex(std::string URI, DocumentVector index)
	{
		std::string deleteQuery = "DELETE FROM indices WHERE uri_id = (SELECT uri_id FROM uris "
				"WHERE uri = '" + URI + "')";
		handleNonRowReturningQuery(deleteQuery);
		std::string query = "INSERT INTO indices VALUES ";
		for (DocumentVector::iterator i = index.begin(); i != index.end(); i++)
		{
			std::ostringstream ossCount;
			ossCount << i->second;
			query += "((SELECT uri_id FROM uris WHERE uri = '" + URI + "'), '" + i->first + "'," + ossCount.str() + ")";
			if (i != index.end())
			{
				query += ",\n";
			}
		}
		handleNonRowReturningQuery(query.substr(0, query.size() - 2));
	}

	void DatabaseHandler::UpdateURIInList(URIPacket::URIPacketPtr element)
	{
		if (!element->URI.empty())
		{
			if (URIInList(element->URI))
			{
				std::ostringstream ossRelevance;
				ossRelevance << element->Relevance;
				std::string query = "UPDATE uris SET relevance = " + ossRelevance.str() + " WHERE uri = '"
						+ element->URI + "'";
				handleNonRowReturningQuery(query);
			}
		}
	}

	URIPacket::URIPacketPtr DatabaseHandler::RetrieveURIFromQueue()
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

	URIPacket::URIPacketPtr DatabaseHandler::RetrieveAndDeleteURIFromQueue()
	{
		std::shared_ptr<URIPacket> result(new URIPacket);
		result = RetrieveURIFromQueue();
		if (!(result->URI.empty()))
		{
			DeleteURIFrom(result->URI, "uri_queue");
		}
		return result;
	}

	URIPacket::URIPacketVector DatabaseHandler::GetURIListFromQueue(const int amount)
	{
		std::vector<std::shared_ptr<URIPacket>> CachedURIQueue;
		SQLHANDLE statementHandle = createStatementHandler();

		std::ostringstream ossAmount;
		ossAmount << amount;
		std::string query = "SELECT uris.uri, uri_queue.priority FROM uris, uri_queue"
				" WHERE uris.uri_id = uri_queue.uri_id ORDER BY uri_queue.priority DESC LIMIT " + ossAmount.str();

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

	DocumentPacket::DocumentPacketPtr DatabaseHandler::RetrieveDocumentFromQueue()
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

	DocumentPacket::DocumentPacketPtr DatabaseHandler::RetrieveAndDeleteDocumentFromQueue()
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
		releaseStatementHandler(stmtHndl);
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
				releaseStatementHandler(stmtHndl);
				return false;
			}
		}
		releaseStatementHandler(stmtHndl);
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

	bool DatabaseHandler::handleNonRowReturningQuery(std::string SQLQuery)
	{
		SQLHANDLE statementHandler = createStatementHandler();
		if (SQL_SUCCESS != SQLExecDirect(statementHandler, (SQLCHAR*) SQLQuery.c_str(), SQL_NTS))
		{
			showError(SQL_HANDLE_STMT, statementHandler);
			releaseStatementHandler(statementHandler);
			return false;
		}
		releaseStatementHandler(statementHandler);
		return true;
	}

	bool DatabaseHandler::executeQuery(std::string SQLQuery, SQLHANDLE& statementHandle)
	{
		if (SQL_SUCCESS != SQLExecDirect(statementHandle, (SQLCHAR*) SQLQuery.c_str(), SQL_NTS))
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
			std::string::size_type statePos = sqlStateString.find(SQL_ERRORSTATE_UNIQUE);
			if (statePos == std::string::npos)
			{
				// Once the library has been updated, this will be turned into a logger message.
				std::cout << "Message: " << message << " SQLSTATE: " << sqlstate << std::endl;
			}
			else
			{
				std::cout << "Duplicate key detected" << std::endl;
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
