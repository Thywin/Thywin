/*
 * Communicator.h
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include <string>

#include "Document.h"
#include "URIRelevance.h"

namespace thywin
{

class Communicator
{
public:
	Communicator(std::string documentQueue, std::string URLQueue, std::string indexStore);
	virtual ~Communicator();

	Document GetDocumentFromQueue();
	void StoreIndex();
	void StoreExpectedURIRelevance(URIRelevance uriRelevance);
	void StoreActualURIRelevance(URIRelevance uriRelevance);
};

} /* namespace thywin */

#endif /* COMMUNICATOR_H_ */
