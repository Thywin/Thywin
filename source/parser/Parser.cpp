/*
 * Parser.cpp
 *
 *  Created on: May 8, 2014
 *      Author: Imre Woudstra
 */

#include <iostream>

#include "Parser.h"
#include "HTMLFileParser.h"
#include "DocumentVector.h"

namespace thywin
{

	Parser::Parser(std::string documentQueue, std::string URLQueue, std::string indexStore) :
			logger("parser.log"), communicator(documentQueue, URLQueue, indexStore)
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
			DocumentPacket documentFromQueue = communicator.GetDocumentFromQueue();

			std::vector<std::string> URIs = parser.ExtractURIs(documentFromQueue.Document, documentFromQueue.URI);

			std::stringstream logMessageURIsFound;
			logMessageURIsFound << "URIs found on URI: " << documentFromQueue.URI << " Count: " << URIs.size();
			logger.Log(INFO, logMessageURIsFound.str());

			std::string text = parser.ExtractText(documentFromQueue.Document);

			DocumentVector docVector(text);
			double relevance = docVector.CalculateSimilarity(subject);

			std::stringstream logMessageRelevance;
			logMessageRelevance << "Relevance of current URI: " << documentFromQueue.URI << " Relevance" << relevance;
			logger.Log(INFO, logMessageRelevance.str());

			for (unsigned int i = 0; i < URIs.size(); i++)
			{
				URIPacket uriPacket;
				uriPacket.Relevance = relevance;
				uriPacket.URI = URIs.at(i);
				communicator.StoreExpectedURIRelevance(uriPacket);
			}
		}
	}

	void Parser::Stop()
	{
		running = false;
	}

} /* namespace thywin */
