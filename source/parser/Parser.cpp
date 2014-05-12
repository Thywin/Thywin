/*
 * Parser.cpp
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#include "Parser.h"
#include "HTMLFileParser.h"
#include <iostream>

namespace thywin
{

	Parser::Parser(std::string documentQueue, std::string URLQueue, std::string indexStore) :
			communicator(documentQueue, URLQueue, indexStore)
	{
		running = false;
	}

	Parser::~Parser()
	{
	}

	void Parser::Run()
	{
		HTMLFileParser parser;

		running = true;
		while (running)
		{
			Document doc = communicator.GetDocumentFromQueue();

			std::vector<std::string> URIs = parser.ExtractURIs(doc.content, doc.URI);
			std::string text = parser.ExtractText(doc.content);

			std::cout << URIs.size() << " URI's found" << std::endl;
			for (unsigned int i = 0; i < URIs.size(); i++)
			{
				communicator.StoreExpectedURIRelevance(URIRelevance(0, URIs.at(i)));
			}
		}
	}

	void Parser::Stop()
	{
		running = false;
	}

} /* namespace thywin */
