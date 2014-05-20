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
		SQLHANDLE stmtHndl;
		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, connectionHandle, &stmtHndl))
		{
			show_error(SQL_HANDLE_STMT, stmtHndl);
			Disconnect(stmtHndl);
			return;
		}
		//std::string query = "INSERT INTO document_queue VALUES ((SELECT uri_id FROM uris WHERE uri = '" + URI + "'), '"
		//		+ content + "')";
		char* statement = (char*)"INSERT INTO document_queue VALUES ((SELECT uri_id FROM uris WHERE uri = '?'), '?')";
		if (SQL_SUCCESS != SQLPrepare(stmtHndl, (SQLCHAR *) statement, strlen(statement)))
		{
			show_error(SQL_HANDLE_STMT, stmtHndl);
			Disconnect(stmtHndl);
			return;
		}
		SQLLEN nts = SQL_NTS;
		SQLBindParameter(stmtHndl, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, input->URI.size(), 0,
				(char *) input->URI.c_str(), 0, &nts);
		SQLBindParameter(stmtHndl, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, input->Document.size(), 0,
				(char *) input->Document.c_str(), 0, &nts);
		if (SQL_SUCCESS != SQLExecute(stmtHndl))
		{
			SQLCHAR sqlstate[1024];
			SQLCHAR message[1024];
			if (SQL_SUCCESS == SQLGetDiagRec(SQL_HANDLE_STMT, stmtHndl, 1, sqlstate, NULL, message, 1024, NULL))
			{
				std::string sqlStateString((char *) sqlstate);
				std::string::size_type statePos = sqlStateString.find("23505");
				if (statePos == std::string::npos)
				{
					std::cout << "Message: " << message << " SQLSTATE: " << sqlstate << std::endl;
					Disconnect(stmtHndl);
					return;
				}
			}
		}
		SQLFreeHandle(SQL_HANDLE_STMT, stmtHndl);
		//handleNonRowReturningQuery(query);
	}

	void DatabaseHandler::AddWordcountToIndex(std::string URI, std::string word, int count)
	{
		std::ostringstream ossCount;
		ossCount << count;
		std::string sCount = ossCount.str();
		std::string query = "INSERT INTO indices VALUES ((SELECT uri_id FROM uris WHERE uri = '" + URI + "'), '" + word
				+ "'," + sCount + ")";
		handleNonRowReturningQuery(query);
	}

	std::shared_ptr<URIPacket> DatabaseHandler::RetrieveURIFromQueue()
	{
		SQLHANDLE stmtHndl;
		std::shared_ptr<URIPacket> result(new URIPacket);
		std::string query = "SELECT uris.uri, uri_queue.priority FROM uris, uri_queue"
				" WHERE uris.uri_id = uri_queue.uri_id LIMIT 1";
		result->URI = "";
		result->Relevance = 0;

		if (executeQuery(query,stmtHndl))
		{
			char uri[1024];
			double priority;
			int rowCount = 0;
			while (SQLFetch(stmtHndl) == SQL_SUCCESS)
			{
				SQLGetData(stmtHndl, 1, SQL_C_CHAR, uri, 1024, NULL);
				SQLGetData(stmtHndl, 2, SQL_C_DOUBLE, &priority, 0, NULL);
				rowCount++;
			}
			if (rowCount >= 1)
			{
				result->URI = std::string(uri);
				result->Relevance = priority;
			}
		}
		SQLFreeHandle(SQL_HANDLE_STMT, stmtHndl);
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
		SQLHANDLE stmtHndl;
		std::shared_ptr<DocumentPacket> result(new DocumentPacket);
		std::string query = "SELECT uris.uri, document_queue.content FROM uris, "
				"document_queue WHERE uris.uri_id = document_queue.uri_id LIMIT 1";
		result->URI = "";
		result->Document = "";

		if (executeQuery(query,stmtHndl))
		{
			char uri[1024];
			std::string content;
			char buffer[2];
			int rowCount = 0;
			while (SQLFetch(stmtHndl) == SQL_SUCCESS)
			{
				SQLRETURN retcode = 0;
				SQLGetData(stmtHndl, 1, SQL_C_CHAR, uri, 1024, NULL);
				do
				{
					retcode = SQLGetData(stmtHndl, 2, SQL_C_CHAR, &buffer, 2, NULL);
					content.push_back(buffer[0]);
				} while (retcode != SQL_SUCCESS);
				content.push_back('\0');
				rowCount++;
			}
			if (rowCount >= 1)
			{
				result->URI = uri;
				result->Document = content;
			}
		}
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

	bool DatabaseHandler::IsQueueEmpty(std::string queue)
	{
		SQLHANDLE stmtHndl;
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

	void DatabaseHandler::handleNonRowReturningQuery(std::string SQLQuery)
	{
		SQLHANDLE stmtHndl;
		if (!connectionCheck())
		{
			return;
		}
		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, connectionHandle, &stmtHndl))
		{
			show_error(SQL_HANDLE_STMT, stmtHndl);
			Disconnect(stmtHndl);
			return;
		}

		const char* query = SQLQuery.c_str();
		if (SQL_SUCCESS != SQLExecDirect(stmtHndl, (SQLCHAR*) query, SQL_NTS))
		{
			SQLCHAR sqlstate[1024];
			SQLCHAR message[1024];
			if (SQL_SUCCESS == SQLGetDiagRec(SQL_HANDLE_STMT, stmtHndl, 1, sqlstate, NULL, message, 1024, NULL))
			{
				std::string sqlStateString((char *) sqlstate);
				std::string::size_type statePos = sqlStateString.find("23505");
				if (statePos == std::string::npos)
				{
					std::cout << "Message: " << message << " SQLSTATE: " << sqlstate << std::endl;
					Disconnect(stmtHndl);
					return;
				}
			}
		}
//		else
//		{
//			SQLLEN affectedRows;
//
//			if (SQL_SUCCESS == SQLRowCount(statementHandle, &affectedRows))
//			{
//				std::string row = affectedRows == 1 ? "row" : "rows";
//				std::cout << affectedRows << " " << row << " affected." << std::endl;
//			}
//		}
		SQLFreeHandle(SQL_HANDLE_STMT, stmtHndl);
	}

	bool DatabaseHandler::executeQuery(std::string SQLQuery, SQLHANDLE& stmtHndl)
	{
		if (!connectionCheck())
		{
			return false;
		}
		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, connectionHandle, &stmtHndl))
		{
			show_error(SQL_HANDLE_STMT, stmtHndl);
			Disconnect(stmtHndl);
			return false;
		}
		const char* query = SQLQuery.c_str();
		if (SQL_SUCCESS != SQLExecDirect(stmtHndl, (SQLCHAR*) query, SQL_NTS))
		{
			show_error(SQL_HANDLE_STMT, stmtHndl);
			return false;
		}
		SQLFreeHandle(SQL_HANDLE_STMT, stmtHndl);
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

	void DatabaseHandler::show_error(unsigned int handletype, const SQLHANDLE& handle)
	{
		SQLCHAR sqlstate[1024];
		SQLCHAR message[1024];
		if (SQL_SUCCESS == SQLGetDiagRec(handletype, handle, 1, sqlstate, NULL, message, 1024, NULL))
			std::cout << "Message: " << message << " SQLSTATE: " << sqlstate << std::endl;
	}
}
