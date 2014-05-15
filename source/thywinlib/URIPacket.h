/*
 * URIElement.h
 *
 *  Created on: May 13, 2014
 *      Author: damonk
 */

#ifndef URIELEMENT_H_
#define URIELEMENT_H_
#include "TPObject.h"

namespace thywin
{

	class URIPacket: public TPObject
	{
		public:
			double Relevance;
			std::string URI;

			std::string Serialize();
			void Deserialize(std::string);
	};
} /* namespace thywin */

#endif /* URIELEMENT_H_ */
