/*
 * Parser.cpp
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#include "Parser.h"
#include "HTMLFileParser.h"

namespace thywin
{

Parser::Parser(std::string documentQueue, std::string URLQueue,
		std::string indexStore) :
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
		for(int i = 0; i < 100; i++)
		{
		Document doc = communicator.GetDocumentFromQueue();
		printf("Got url: %s \n", doc.location.c_str());
		printf("Last 100: %s \n\n", doc.content.substr(doc.content.size() - 100, 100).c_str());

		std::vector<std::string> URIs = parser.ExtractURIs(doc.content);
		std::string text = parser.ExtractText(doc.content);
		}
		running = false;
	}
}

} /* namespace thywin */
