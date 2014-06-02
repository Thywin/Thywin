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
#include <DocumentVector.h>
#include <sstream>

namespace thywin
{
	bool SearchEngine::compareURIsWithRelevance(std::pair<std::string, double> first, std::pair<std::string, double> second)
	{
		return first.second < second.second;
	}
	
	SearchEngine::SearchEngine() : logger("SearchEngine.log")
	{
		// TODO Auto-generated constructor stub
		
	}
	
	SearchEngine::SearchEngine(const SearchEngine& searchEngine) : logger("SearchEngine.log")
	{
		
	}
	
	SearchEngine::~SearchEngine()
	{
		// TODO Auto-generated destructor stub
	}
	
	MultiURIPacket SearchEngine::Search(std::string searchWords)
	{
		MultiURIPacket searchResults;
		
		//std::string searchWords("probabilistic logic discrete event simulation");
		DocumentVector searchVector(searchWords);
		
		DatabaseHandler databaseHandler;
		SQLHANDLE sqlHandle = databaseHandler.createStatementHandler();
		std::map<std::string, DocumentVector> indices;
		std::stringstream searchQueryStream;
		searchQueryStream << "SELECT uri, keyword, number_of_occurences "
				"FROM indices INNER JOIN uris ON indices.uri_id = uris.uri_id "
				"WHERE indices.uri_id IN "
				"(SELECT uri_id "
				"FROM indices "
				"WHERE ";
		bool firstWord = true;
		for (DocumentVector::iterator i = searchVector.begin(); i != searchVector.end(); i++)
		{
			if (!firstWord)
			{
				searchQueryStream << " OR ";
			}
			searchQueryStream << "keyword = '" << i->first << "' ";
			firstWord = false;
		}
		searchQueryStream << ")";
		if (databaseHandler.executeQuery(searchQueryStream.str(), sqlHandle))
		{
			char uri[1024];
			char keyword[1024];
			unsigned int count = 0;
			while (SQLFetch(sqlHandle) == SQL_SUCCESS)
			{
				// Get data from first column
				SQLGetData(sqlHandle, 1, SQL_C_CHAR, uri, 1024, NULL);
				// Get data from second column
				SQLGetData(sqlHandle, 2, SQL_C_CHAR, keyword, 1024, NULL);
				// Get data from third column
				SQLGetData(sqlHandle, 3, SQL_INTEGER, &count, 0, NULL);
				(indices[uri])[keyword] = count;
			}
			std::map<std::string, double> uriWithRelevance;
			for (std::map<std::string, DocumentVector>::iterator i = indices.begin(); i != indices.end(); i++)
			{
				uriWithRelevance[i->first] = i->second.CalculateSimilarity(searchVector);
			}
			const unsigned int maxURIsToReturn = 25;
			
			for (unsigned int i = 0; i < maxURIsToReturn; i++)
			{
				if (uriWithRelevance.empty())
				{
					break;
				}
				URIPacket uriPacket;
				std::map<std::string, double>::iterator maxValue = std::max_element(uriWithRelevance.begin(),
						uriWithRelevance.end(), SearchEngine::compareURIsWithRelevance);
				uriPacket.URI = maxValue->first;
				uriPacket.Relevance = maxValue->second;
				searchResults.Content.push_back(std::shared_ptr<URIPacket>(new URIPacket(uriPacket)));
				uriWithRelevance.erase(maxValue);
				std::cout << uriPacket.URI << "\t" << uriPacket.Relevance << std::endl;
			}
		}
		else
		{
			std::cout << "Failed to execute query";
		}
		return searchResults;
	}

} /* namespace thywin */
