/*
 * ThywinPacket.cpp
 *
 *  Created on: 22 mei 2014
 *      Author: Bobby Bouwmann
 */

#include <memory>
#include "ThywinPacket.h"

namespace thywin
{

	ThywinPacket::ThywinPacket(PacketMethod method, PacketType type, std::shared_ptr<ThywinPacketContent> content)
	{
		Method = method;
		Type = type;
		Content = content;
	}

	ThywinPacket::~ThywinPacket()
	{
	}

}

