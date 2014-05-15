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
	const char SEP = (char) 30;
	const char EOT = (char) 4;
	const char STX = (char) 3;
	class Communicator
	{
		public:
			Communicator(std::string ipaddress);
			virtual ~Communicator();

			int SendPacket(ThywinPacket packet);
			ThywinPacket* ReceivePacket(TPObject* obj);
			void CloseConnection();

		private:
			int connectionSocket;
	};

} /* namespace thywin */

#endif /* COMMUNICATOR_H_ */
