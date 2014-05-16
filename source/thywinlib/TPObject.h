/*
 * TPObject.h
 *
 *  Created on: May 13, 2014
 *      Author: damonk
 */

#ifndef TPOBJECT_H_
#define TPOBJECT_H_

#include <sstream>

namespace thywin
{
	/**
	 * A class that implements TPObject can be sent over a socket via the Serialize and Deserialize methods
	 */
	class TPObject
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
			virtual void Deserialize(std::string input) = 0;

			/**
			 * Empty constructor
			 */
			TPObject()
			{
			}

			/**
			 * Empty destructor
			 */
			virtual ~TPObject()
			{
			}
	};

} /* namespace thywin */

#endif /* TPOBJECT_H_ */
