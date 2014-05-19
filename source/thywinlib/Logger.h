/*
 * Logger.h
 *
 *  Created on: 14 mei 2014
 *      Author: Bobby Bouwmann
 *      Author: Erwin Janssen
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <fstream>
#include <string>

namespace thywin
{

	enum logEnum
	{
		INFO = 0, DEBUG = 1, ERROR = 2, WARNING = 3
	};

	const short TIME_STRING_SIZE = 50;

	class Logger
	{
		public:
			/**
			 * This is the constructor. It creates a Logger and uses the
			 * specified filename to store the logs.
			 *
			 * @param logfileName The logfile
			 */
			Logger(const std::string& logfileName);

			/**
			 * Default destructor
			 */
			virtual ~Logger();

			/**
			 * Default copy constructor
			 */
			Logger(const Logger& logger);

			/**
			 * Log a message to the logfile.
			 *
			 * @param logmessage The logtype based on the logEnum.
			 * @param message The logmessage
			 */
			void Log(const logEnum& logmessage, const std::string& message);
		private:
			std::ofstream logfileStream;
			std::string getLogtype(const logEnum& logmessage);
	};

} /* namespace thywin */

#endif /* LOGGER_H_ */
