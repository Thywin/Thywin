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
	enum logEnum { INFO = 0, DEBUG = 1, ERROR = 2 };

	class Logger
	{
		public:
			Logger(std::string logfileName);
			virtual ~Logger();
			void log(logEnum logmessage, std::string message);
			std::string getLogtype(logEnum logmessage);
		private:
			std::ofstream logfileStream;
	};

} /* namespace thywin */

#endif /* LOGGER_H_ */
