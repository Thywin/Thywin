/*
 * Parser.cpp
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#include <iostream>

#include "Parser.h"
#include "HTMLFileParser.h"
#include "DocumentVector.h"

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
		DocumentVector subject("probabilistic logic discrete event simulation");

		running = true;
		while (running)
		{
			Document doc = communicator.GetDocumentFromQueue();

			std::vector<std::string> URIs = parser.ExtractURIs(doc.content, doc.URI);
			std::cout << URIs.size() << " URI's found" << std::endl;

			std::string text = parser.ExtractText(doc.content);

			DocumentVector docVector(text);
			double relevance = docVector.CalculateSimilarity(&subject);
			std::cout << relevance << " URI relevance" << std::endl;

			for (unsigned int i = 0; i < URIs.size(); i++)
			{
				communicator.StoreExpectedURIRelevance(URIRelevance(relevance, URIs.at(i)));
			}
		}
	}

	void Parser::Stop()
	{
		running = false;
	}

} /* namespace thywin */
