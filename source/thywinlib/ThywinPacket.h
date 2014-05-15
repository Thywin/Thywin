/*
 * ThywinPacket.h
 *
 *  Created on: May 13, 2014
 *      Author: damonk
 */

#ifndef THYWINPACKET_H_
#define THYWINPACKET_H_

#include <sstream>

#include "TPObject.h"

namespace thywin
{
	enum PacketType
	{
		URI = 1, DOCUMENT = 2, RELEVANCE = 3, DOCUMENTVECTOR = 4
	};

	enum PacketMethod
	{
		GET = 1, PUT = 2, RESPONSE = 3
	};

	typedef struct ThywinPacket
	{
			PacketMethod Method;
			PacketType Type;
			TPObject* Content;
	} ThywinPacket;

} /* namespace thywin */

#endif /* THYWINPACKET_H_ */
