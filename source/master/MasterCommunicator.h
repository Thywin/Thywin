/*
 * communicator.hpp
 *
 *  Created on: 10 mei 2014
 *      Author: Thomas Kooi
 */

#ifndef COMMUNICATOR_HPP_
#define COMMUNICATOR_HPP_
#include <memory>
#include "Communicator.h"
#include "URIPacket.h"
#include "DocumentPacket.h"
#include "ThywinPacketContent.h"
#include "Master.h"

namespace thywin
{
	class MasterCommunicator
	{
		public:

			/**
			 * Grabs the first element from the URI Queue
			 * and put's this into a new Thywin Packet for a reply to a Get Request.
			 *
			 * @Return ThywinPacket with the following values:
			 * 	Method RESPONSE
			 * 	Type URI
			 * 	Content First URI in Queue
			 */
			ThywinPacket HandleGetURI();

			/**
			 * Grabs the first element from the Document Queue
			 * and put's this into a new Thywin Packet for a reply to a Get Request.
			 *
			 * @Return ThywinPacket with the following values:
			 * 	Method RESPONSE
			 * 	Type DOCUMENT
			 * 	Content First Document in Queue
			 */
			ThywinPacket HandleGetDocument();

			/**
			 * Wrapper for putting in new URIs into the URI queue.
			 * Used as a layer between the Client Connection and Queue
			 * @param Content shared_ptr<TPObject>, expected to be an URIPacket
			 */
			void HandlePutURI(std::shared_ptr<ThywinPacketContent> Content);

			/**
			 * Wrapper for putting in new Documents into the Document queue.
			 * Used as a layer between the Client Connection and Queue
			 * @param Content shared_ptr<TPObject>, expected to be an DocumentPacket
			 */
			void HandlePutDocument(std::shared_ptr<ThywinPacketContent> Content);

			/**
			 * Wrapper for putting multiple URIs into the URIs queue.
			 * Used as a layer between the Client Connection and Queue.
			 * @param Content shared_ptr<ThywinPacketContent>, expected to be an MultiURIPacket
			 */
			void HandlePutUriVector(std::shared_ptr<ThywinPacketContent> Content);

		private:
			ThywinPacket createResponsePacket(std::shared_ptr<ThywinPacketContent> content);
	};

}

#endif /* COMMUNICATOR_HPP_ */
