/*
 * Logger.h
 *
 *  Created on: 14 mei 2014
 *      Author: Erwin
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <fstream>
#include <string>

namespace thywin
{
	
	class Logger
	{
		public:
			Logger(std::string logfileName);
			virtual ~Logger();
			void log(std::string message);
		private:
			std::ofstream logfileStream;
	};

} /* namespace thywin */

#endif /* LOGGER_H_ */
