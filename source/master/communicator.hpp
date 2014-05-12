/*
 * communicator.hpp
 *
 *  Created on: 10 mei 2014
 *      Author: Thomas
 */

#ifndef COMMUNICATOR_HPP_
#define COMMUNICATOR_HPP_

namespace thywin
{
	const short TYPE_URI = 1;			// Used in the Thywin packet to identify the type of packet.
	const short TYPE_DOCUMENT = 2;		// Used in the Thywin packet to identify the type of packet.
	const short ACTION_GET = 1;			// Used in the Thywin packet to identify the action requested by client
	const short ACTION_PUT = 2;			// Used in the Thywin packet to identify the action requested by client
	struct ThywinPacket
	{
			short type;
			short action;
			int size;
	};
	class Communicator
	{
		public:
			/*
			 * Creates a pre-threaded server on given port. Returns the Socket Descriptor of the new server
			 * @Port		Valid port number. Can be any value between # and #.
			 * @return		Socket Descriptor or -1 in case of error
			 */
			int SetupServer(int port);

			/*
			 * Handles all incomming connection on communicator server
			 * @int socket	Server Socket descriptor
			 */
			void HandleConnection(int socket);

		private:
			void handleGetURI(int socket);
			void handleGetDocument(int socket);
			void handlePutURI(int socket, ThywinPacket container);
			void handlePutDocument(int socket, ThywinPacket container);
	};

}

#endif /* COMMUNICATOR_HPP_ */
