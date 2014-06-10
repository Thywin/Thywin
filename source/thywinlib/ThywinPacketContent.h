/*
 * ThywinPacketContent.h
 *
 *  Created on: May 13, 2014
 *      Author: Thomas Kooi
 *    	Author: Imre Woudstra
 */

#ifndef THYWINPACKETCONTENT_H_
#define THYWINPACKETCONTENT_H_

#include <sstream>

namespace thywin
{
	/**
	 * A class that implements TPObject can be sent over a socket via the Serialize and Deserialize methods
	 */
	class ThywinPacketContent
	{
		public:
			/**
			 * Virtual method to serialize a TPObject which will be sent over a socket.
			 * @return a string that represents the serialized object.
			 */
			virtual std::string Serialize() = 0;

			/**
			 * Virtual method to deserialize a String to a TPObject which was sent over a socket.
			 * @input the string that represents the serialized object
			 */
			virtual void Deserialize(const std::string& input) = 0;

			/**
			 * Empty constructor
			 */
			ThywinPacketContent()
			{
			}

			/**
			 * Empty destructor
			 */
			virtual ~ThywinPacketContent()
			{
			}
	};

} /* namespace thywin */

#endif /* THYWINPACKETCONTENT_H_ */
