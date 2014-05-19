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
			void AddDocumentToQueue(std::string URI, std::string content);
			void AddWordcountToIndex(std::string URI, std::string word, int count);
			std::vector<std::string> RetrieveURIFromQueue();
			std::vector<std::string> RetrieveAndDeleteURIFromQueue();
			std::vector<std::string> RetrieveDocumentFromQueue();
			std::vector<std::string> RetrieveAndDeleteDocumentFromQueue();
			bool IsQueueEmpty(std::string queue);
			void Disconnect();
		private:
			void handleNonRowReturningQuery(std::string query);
			bool executeQuery(std::string query);
			bool connectionCheck();
			void show_error(unsigned int handletype, const SQLHANDLE& handle);
	};

} /* namespace thywin */

#endif /* DATABASEHANDLER_H_ */
