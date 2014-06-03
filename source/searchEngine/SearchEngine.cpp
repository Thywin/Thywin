/*
 * SearchEngine.cpp
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin
 */

#include "SearchEngine.h"
#include <algorithm>
#include <iostream>
#include <Logger.h>
#include <map>
#include <sqlext.h>
#include <sql.h>
#include "DatabaseHandler.h"
#include <sstream>

namespace thywin
{
	bool SearchEngine::compareURIsWithRelevance(std::pair<std::string, double> first,
			std::pair<std::string, double> second)
	{
		return first.second < second.second;
	}
	
	SearchEngine::SearchEngine() :
			logger("SearchEngine.log")
	{
	}
	
	SearchEngine::SearchEngine(const SearchEngine& searchEngine) :
			logger("SearchEngine.log")
	{
	}
	
	SearchEngine::~SearchEngine()
	{
	}
	
	MultiURIPacket SearchEngine::Search(const std::string& searchTerms)
	{
		logger.Log(INFO, "Received search words: " + searchTerms);
		
		DocumentVector searchVector(searchTerms);
		MultiURIPacket searchResults;
		
		DatabaseHandler databaseHandler;
		SQLHANDLE sqlHandle = databaseHandler.createStatementHandler();
		
		if (databaseHandler.executeQuery(constructSearchQuery(searchVector), sqlHandle))
		{
			std::map<std::string, DocumentVector> retrievedIndices;
			
			char uri[URI_FIELD_SIZE];
			char keyword[KEYWORD_FIELD_SIZE];
			unsigned int numberOfOccurences = 0;
			
			while (SQLFetch(sqlHandle) == SQL_SUCCESS)
			{
				// Get data from first column, the URI
				SQLGetData(sqlHandle, 1, SQL_CHAR, uri, URI_FIELD_SIZE, NULL);
				// Get data from second column, the keyword
				SQLGetData(sqlHandle, 2, SQL_CHAR, keyword, KEYWORD_FIELD_SIZE, NULL);
				// Get data from third column, the number_of_occurences
				SQLGetData(sqlHandle, 3, SQL_INTEGER, &numberOfOccurences, 0, NULL);
				(retrievedIndices[uri])[keyword] = numberOfOccurences;
			}
			
			std::map<std::string, double> uriWithRelevance;
			for (std::map<std::string, DocumentVector>::iterator i = retrievedIndices.begin();
					i != retrievedIndices.end(); i++)
			{
				uriWithRelevance[i->first] = i->second.CalculateSimilarity(searchVector);
			}
			
			for (unsigned int i = 0; i < MAX_URIS_TO_RETURN; i++)
			{
				if (uriWithRelevance.empty())
				{
					break;
				}
				else
				{
					URIPacket uriPacket;
					std::map<std::string, double>::iterator mostRelevantURI = std::max_element(uriWithRelevance.begin(),
							uriWithRelevance.end(), SearchEngine::compareURIsWithRelevance);
					uriPacket.URI = mostRelevantURI->first;
					uriPacket.Relevance = mostRelevantURI->second;
					searchResults.Content.push_back(std::shared_ptr<URIPacket>(new URIPacket(uriPacket)));
					uriWithRelevance.erase(mostRelevantURI);
				}
			}
		}
		else
		{
			logger.Log(ERROR, "Failed to execute query");
		}
		
		databaseHandler.releaseStatementHandler(sqlHandle);
		return searchResults;
	}
	
	std::string SearchEngine::constructSearchQuery(DocumentVector searchVector)
	{
		std::stringstream searchQueryStream;
		searchQueryStream << "SELECT uri, keyword, number_of_occurences "
				"FROM indices INNER JOIN uris ON indices.uri_id = uris.uri_id "
				"WHERE indices.uri_id IN "
				"(SELECT uri_id "
				"FROM indices "
				"WHERE ";
		
		bool firstSearchWord = true;
		for (DocumentVector::iterator i = searchVector.begin(); i != searchVector.end(); i++)
		{
			if (!firstSearchWord)
			{
				searchQueryStream << " OR ";
			}
			searchQueryStream << "keyword = '" << i->first << "' ";
			firstSearchWord = false;
		}
		searchQueryStream << ")";
		
		std::string searchQuery = searchQueryStream.str();
		logger.Log(INFO, "Constructed search query: " + searchQuery);
		return searchQuery;
	}

} /* namespace thywin */
