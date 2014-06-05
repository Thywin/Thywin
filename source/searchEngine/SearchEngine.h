/*
 * SearchEngine.h
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin Janssen
 */

#ifndef SEARCHENGINE_H_
#define SEARCHENGINE_H_

#include <string>

#include <DocumentVector.h>
#include <Logger.h>
#include "MultiURIPacket.h"

namespace thywin
{
	/**
	 * This search engine class can be used to retrieve to most relevant results
	 * for a given set of search terms
	 */
	class SearchEngine
	{
		public:
			/**
			 * Default constructor, also creates a logger.
			 */
			SearchEngine();

			/**
			 * Default copy constructor, also creates a logger.
			 */
			SearchEngine(const SearchEngine& searchEngine);
			
			/*
			 * Default destructor
			 */
			virtual ~SearchEngine();

			/**
			 * Searches the database for the URIs that are most relevant with the search words passed as parameter.
			 * @param searchWords The words that will be used for findig relevant URIs 
			 */
			MultiURIPacket Search(const std::string& searchTerms);
			
		private:
			Logger logger;
			static const unsigned int URI_FIELD_SIZE = 1024;
			static const unsigned int KEYWORD_FIELD_SIZE = 1024;
			static const unsigned int MAX_URIS_TO_RETURN = 25;
			std::string constructSearchQuery(DocumentVector searchVector);
			static bool compareURIsWithRelevance(std::pair<std::string, double> first,
					std::pair<std::string, double> second);
			
	};

} /* namespace thywin */

#endif /* SEARCHENGINE_H_ */
