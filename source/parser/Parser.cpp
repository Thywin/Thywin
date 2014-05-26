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
#include "DocumentVectorPacket.h"

namespace thywin
{

	Parser::Parser(const std::string& masterIP, const unsigned short masterPort) :
			logger("parser.log"), communicator(masterIP, masterPort)
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
			DocumentPacket documentToParse = communicator.GetDocumentFromQueue();

			FileParser::URIs extractedURIs = parser.ExtractURIs(documentToParse.Document, documentToParse.URI);

			std::stringstream logMessageURIsFound;
			logMessageURIsFound << "URIs found on URI: " << documentToParse.URI << " Count: " << extractedURIs.size();
			logger.Log(INFO, logMessageURIsFound.str());

			std::string text = parser.ExtractText(documentToParse.Document);

			DocumentVector docVector(text);
			double relevance = docVector.CalculateSimilarity(subject);

			DocumentVectorPacket documentVectorPacket(documentToParse.URI,relevance,docVector);
			communicator.StoreIndex(documentVectorPacket);

			std::stringstream logMessageRelevance;
			logMessageRelevance << "Relevance of current URI: " << documentToParse.URI << " Relevance: " << relevance;
			logger.Log(INFO, logMessageRelevance.str());

			for (unsigned int i = 0; i < extractedURIs.size(); i++)
			{
				URIPacket uriPacket;
				uriPacket.Relevance = relevance;
				uriPacket.URI = extractedURIs.at(i);
				communicator.StoreExpectedURIRelevance(uriPacket);
			}
		}
	}

	void Parser::Stop()
	{
		running = false;
	}

} /* namespace thywin */
