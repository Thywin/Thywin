/*
 * Document.h
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <string>

namespace thywin
{

typedef struct Document
{
	std::string content;
	std::string location;
} Document;

} /* namespace thywin */

#endif /* DOCUMENT_H_ */
