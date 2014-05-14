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
	class TPObject
	{
		public:
			virtual std::string Serialize() = 0;
			virtual void Deserialize(std::string) = 0;

			TPObject()
			{
			}
			virtual ~TPObject()
			{
			}
	};

} /* namespace thywin */

#endif /* TPOBJECT_H_ */
