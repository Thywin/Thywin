/*
 * Document.h
 *
 *  Created on: May 13, 2014
 *      Author: damonk
 */

#ifndef DOCUMENT_H_
#define DOCUMENT_H_
#include "TPObject.h"

namespace thywin
{

	class DocumentPacket: public TPObject
	{
		public:
			std::string Serialize();
			void Deserialize(std::string);
			int URISize;
			std::string URI;
			int DocumentSize;
			std::string Document;
	};

} /* namespace thywin */

#endif /* DOCUMENT_H_ */
