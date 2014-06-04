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
	
	bool DatabaseHandler::executeQuery(std::string SQLQuery, SQLHANDLE& statementHandle)
	{
		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, connectionHandle, &statementHandle))
		{
			showError(SQL_HANDLE_STMT, statementHandle);
			Disconnect(statementHandle);
			throw std::runtime_error(std::string("Couldn't allocate statement handle"));
		}
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
}
