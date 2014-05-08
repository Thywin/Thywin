/*
 * Communicator.cpp
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#include "Communicator.h"

namespace thywin
{

Communicator::Communicator(std::string documentQueue, std::string URLQueue,
		std::string indexStore)
{
}

Communicator::~Communicator()
{
}

Document Communicator::GetDocumentFromQueue()
{
	Document doc;

	return doc;
}

void Communicator::StoreIndex()
{

}

void Communicator::StoreExpectedURIRelevance(URIRelevance uriRelevance)
{

}

void Communicator::StoreActualURIRelevance(URIRelevance uriRelevance)
{

}

} /* namespace thywin */
