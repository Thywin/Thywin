/*
 * DatabaseHandler.cpp
 *
 *  Created on: May 15, 2014
 *      Author: Thomas Gerritsen
 */
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include "DatabaseHandler.h"

namespace thywin
{
	SQLHANDLE environmentHandle;
	SQLHANDLE connectionHandle;
	std::string ip;
	int port = 0;
	bool connected = false;

	DatabaseHandler::DatabaseHandler(std::string ipaddress)
	{
		ip = ipaddress;
		port = 5432;
	}

	DatabaseHandler::DatabaseHandler(std::string ipaddress, int givenPort)
	{
		ip = ipaddress;
		port = givenPort;
	}

	void DatabaseHandler::Connect()
	{
		connected = true;
		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &environmentHandle))
		{
			Disconnect();
			return;
		}

		if (SQL_SUCCESS != SQLSetEnvAttr(environmentHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0))
		{
			Disconnect();
			return;
		}

		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, environmentHandle, &connectionHandle))
		{
			Disconnect();
			return;
		}

		SQLSetConnectOption(connectionHandle, SQL_LOGIN_TIMEOUT, 15);

		std::ostringstream ossPort;
		ossPort << port;
		std::string sPort = ossPort.str();
		std::string temp = "DRIVER={/usr/lib/arm-linux-gnueabihf/odbc/psqlodbca.so};SERVER=" + ip + ";PORT=" + sPort
				+ ";DATABASE=thywin;UID=thywin;PWD=hanicampna;";
		const char* connectionString = temp.c_str();

		SQLCHAR retconstring[1024];
		SQLRETURN connect = SQLDriverConnect(connectionHandle,
		NULL, (SQLCHAR*) connectionString,
		SQL_NTS, retconstring, 1024,
		NULL,
		SQL_DRIVER_NOPROMPT);

		switch (connect)
		{
			case SQL_SUCCESS_WITH_INFO:
				show_error(SQL_HANDLE_DBC, connectionHandle);
				break;
			case SQL_INVALID_HANDLE:
			case SQL_ERROR:
				show_error(SQL_HANDLE_DBC, connectionHandle);
				Disconnect();
				return;
			default:
				break;
		}
		//std::cout << "Connected!" << std::endl;
	}

	void DatabaseHandler::DeleteURIFrom(std::string URI, std::string table, bool all)
	{
		std::string query = "";
		if (all)
		{
			query = "DELETE FROM " + table + " WHERE uri_id = (SELECT uri_id FROM uris WHERE uri = '" + URI + "')";
		}
		else
		{
			query = "DELETE FROM " + table + " WHERE ctid IN (SELECT ctid FROM " + table
					+ " WHERE uri_id = (SELECT uri_id FROM uris WHERE uri = '" + URI + "') LIMIT 1)";
		}
		handleNonRowReturningQuery(query);
	}

	void DatabaseHandler::AddURIToList(std::shared_ptr<URIPacket> element)
	{
		std::ostringstream ossRelevance;
		ossRelevance << element->Relevance;
		std::string sRelevance = ossRelevance.str();
		std::string query = "INSERT INTO uris (uri, relevance) VALUES ('" + element->URI + "'," + sRelevance + ")";
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
			show_error(SQL_HANDLE_STMT, statementHandle);
			Disconnect(statementHandle);
			return; // throw exception
		}
		SQLLEN nts = SQL_NTS;
		SQLBindParameter(statementHandle, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, input->URI.size(), 0,
				(char *) input->URI.c_str(), input->URI.size(), &nts);
		SQLBindParameter(statementHandle, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, input->Document.size(), 0,
				(char *) input->Document.c_str(), input->Document.size(), &nts);
		if (SQL_SUCCESS != SQLExecute(statementHandle))
		{
			show_error(SQL_HANDLE_STMT, statementHandle);
		}
		releaseStatementHandler(statementHandle);
	}

	void DatabaseHandler::AddWordcountToIndex(std::string URI, std::string word, int count)
	{
		std::ostringstream ossCount;
		ossCount << count;  // std::string(count); // Possible implementation to use
		std::string sCount = ossCount.str();
		std::string query = "INSERT INTO indices VALUES ((SELECT uri_id FROM uris WHERE uri = '" + URI + "'), '" + word
				+ "'," + sCount + ")";
		handleNonRowReturningQuery(query);
	}

	std::shared_ptr<URIPacket> DatabaseHandler::RetrieveURIFromQueue()
	{
		SQLHANDLE statementHandle = createStatementHandler();
		std::shared_ptr<URIPacket> result(new URIPacket);
		std::string query = "SELECT uris.uri, uri_queue.priority FROM uris, uri_queue"
				" WHERE uris.uri_id = uri_queue.uri_id LIMIT 1";

		if (executeQuery(query, statementHandle))
		{
			char uri[1024];
			double priority;
			if (SQLFetch(statementHandle) == SQL_SUCCESS)
			{
				SQLGetData(statementHandle, 1, SQL_C_CHAR, uri, 1024, NULL);
				SQLGetData(statementHandle, 2, SQL_C_DOUBLE, &priority, 0, NULL);
				result->URI = std::string(uri);
				result->Relevance = priority;
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
			DeleteURIFrom(result->URI, "uri_queue", false);
		}
		return result;
	}

	std::shared_ptr<DocumentPacket> DatabaseHandler::RetrieveDocumentFromQueue()
	{
		SQLHANDLE statementHandler = createStatementHandler();
		std::shared_ptr<DocumentPacket> result(new DocumentPacket);
		std::string query = "SELECT uris.uri, document_queue.content FROM uris, "
				"document_queue WHERE uris.uri_id = document_queue.uri_id LIMIT 1";

		if (executeQuery(query, statementHandler))
		{
			char uri[1024];
			char buffer[2];
			if (SQLFetch(statementHandler) == SQL_SUCCESS)
			{
				SQLRETURN retcode = 0;
				if (SQLGetData(statementHandler, 1, SQL_C_CHAR, uri, 1024, NULL) == SQL_SUCCESS) {
					result->URI = std::string(uri);
					do
					{
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
		std::shared_ptr<DocumentPacket> result(new DocumentPacket);
		result = RetrieveDocumentFromQueue();
		if (!(result->URI.empty()))
		{
			DeleteURIFrom(result->URI, "document_queue", false);
		}
		return result;
	}

	int DatabaseHandler::GetRowCount(std::string queue)
	{
		SQLHANDLE stmtHndl = createStatementHandler();
		std::string query = "SELECT count(uri_id) FROM " + queue;
		if (executeQuery(query, stmtHndl))
		{
			if (SQLFetch(stmtHndl) == SQL_SUCCESS)
			{
				int rowsInQueue = 0;
				SQLGetData(stmtHndl, 1, SQL_C_LONG, &rowsInQueue, 0, NULL);
				return rowsInQueue;
			}
		}
		return 0;
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

	SQLHANDLE DatabaseHandler::createStatementHandler()
	{
		SQLHANDLE stmtHndl;
		if (!connectionCheck())
		{
			// throw exception
		}

		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, connectionHandle, &stmtHndl))
		{
			show_error(SQL_HANDLE_STMT, stmtHndl);
			Disconnect(stmtHndl);
			// throw exception
		}
		return stmtHndl;
	}

	void DatabaseHandler::releaseStatementHandler(SQLHANDLE& handler)
	{
		if (connectionCheck())
		{
			SQLFreeHandle(SQL_HANDLE_STMT, handler);
		}
	}

	void DatabaseHandler::handleNonRowReturningQuery(std::string SQLQuery)
	{
		SQLHANDLE statementHandler = createStatementHandler();
		const char* query = SQLQuery.c_str();
		if (SQL_SUCCESS != SQLExecDirect(statementHandler, (SQLCHAR*) query, SQL_NTS))
		{
			show_error(SQL_HANDLE_STMT, statementHandler);
		}
		releaseStatementHandler(statementHandler);
	}

	bool DatabaseHandler::executeQuery(std::string SQLQuery, SQLHANDLE& statementHandle)
	{
		if (!connectionCheck())
		{
			return false;
		}
		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, connectionHandle, &statementHandle))
		{
			show_error(SQL_HANDLE_STMT, statementHandle);
			Disconnect(statementHandle);
			return false;
		}
		const char* query = SQLQuery.c_str();
		if (SQL_SUCCESS != SQLExecDirect(statementHandle, (SQLCHAR*) query, SQL_NTS))
		{
			show_error(SQL_HANDLE_STMT, statementHandle);
			return false;
		}
		return true;
	}

	bool DatabaseHandler::connectionCheck()
	{
		if (!connected)
		{
			std::cout << "Not connected to the database. Is the IP address correct "
					"and did you call the \"Connect\" function?" << std::endl;
			return false;
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
			//std::cout << "Disconnected" << std::endl;
		}

	}

	void DatabaseHandler::show_error(unsigned int handletype, const SQLHANDLE& statementHandler)
	{
		SQLCHAR sqlstate[1024];
		SQLCHAR message[1024];
		if (SQL_SUCCESS == SQLGetDiagRec(SQL_HANDLE_STMT, statementHandler, 1, sqlstate, NULL, message, 1024, NULL))
		{
			std::string sqlStateString((char *) sqlstate);
			std::string::size_type statePos = sqlStateString.find("23505");
			if (statePos == std::string::npos)
			{
				std::cout << "Message: " << message << " SQLSTATE: " << sqlstate << std::endl;
			}
		}
	}
}
