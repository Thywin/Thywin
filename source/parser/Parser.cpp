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
	// TODO Auto-generated destructor stub
}

void Parser::Run()
{
	HTMLFileParser parser;

	running = true;
	while (running)
	{
		Document doc = communicator.GetDocumentFromQueue();

		std::vector<std::string> URIs = parser.ExtractURIs(doc.content);
		std::string text = parser.ExtractText(doc.content);
	}
}

} /* namespace thywin */
