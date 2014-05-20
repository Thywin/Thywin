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
			DatabaseHandler(std::string ipaddress);
			DatabaseHandler(std::string ipaddress, int givenPort);
			void Connect();
			void DeleteURIFrom(std::string URI, std::string table, bool all);
			void AddURIToList(std::shared_ptr<URIPacket> element);
			void AddURIToQueue(std::string URI);
			void AddDocumentToQueue(std::shared_ptr<DocumentPacket> input);
			void AddWordcountToIndex(std::string URI, std::string word, int count);
			std::shared_ptr<URIPacket> RetrieveURIFromQueue();
			std::shared_ptr<URIPacket> RetrieveAndDeleteURIFromQueue();
			std::shared_ptr<DocumentPacket> RetrieveDocumentFromQueue();
			std::shared_ptr<DocumentPacket> RetrieveAndDeleteDocumentFromQueue();
			bool IsQueueEmpty(std::string queue);
			void Disconnect(SQLHANDLE& stmtHndl);
			void Disconnect();
		private:
			void handleNonRowReturningQuery(std::string query);
			bool executeQuery(std::string query, SQLHANDLE& stmtHndl);
			bool connectionCheck();
			void show_error(unsigned int handletype, const SQLHANDLE& handle);
	};

} /* namespace thywin */

#endif /* DATABASEHANDLER_H_ */
