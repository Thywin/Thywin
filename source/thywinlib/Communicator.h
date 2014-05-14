/*
 * Communicator.h
 *
 *  Created on: May 13, 2014
 *      Author: damonk
 */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include "ThywinPacket.h"

#include <string>

namespace thywin
{

	class Communicator
	{
		public:
			Communicator(std::string ipaddress);
			virtual ~Communicator();
			
			int SendPacket(ThywinPacket packet);
			ThywinPacket* ReceivePacket(TPObject* obj);
			
		private:
			int connectionSocket;
	};

} /* namespace thywin */

#endif /* COMMUNICATOR_H_ */
