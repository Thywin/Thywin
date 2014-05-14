/*
 * Document.h
 *
 *  Created on: May 13, 2014
 *      Author: damonk
 */

#ifndef DOCUMENT_H_
#define DOCUMENT_H_

namespace thywin
{
	typedef struct DocumentPacket
	{
			int URISize;
			const char* URI;
			int DocumentSize;
			const char* Document;
	} Document;

} /* namespace thywin */

#endif /* DOCUMENT_H_ */
