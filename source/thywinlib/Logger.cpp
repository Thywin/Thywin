/*
 * Logger.cpp
 *
 *  Created on: 14 mei 2014
 *      Author: Bobby Bouwmann
 *      Author: Erwin Janssen
 */

#include "Logger.h"
#include <ctime>

namespace thywin
{

	Logger::Logger(const std::string& logfileName)
	{
		logfileStream.open(logfileName.c_str(), std::ofstream::out | std::ofstream::app);
	}

	Logger::~Logger()
	{
	}

	Logger::Logger(const Logger& logger)
	{
	}

	void Logger::Log(const logEnum& logmessage, const std::string& message)
	{
		char timeString[TIME_STRING_SIZE];
		time_t currentTime = time(NULL);
		struct tm* timeComponents = localtime(&currentTime);
		strftime(timeString, TIME_STRING_SIZE, "%F %T", timeComponents);

		logfileStream << "[" << timeString << "]" << " " << getLogtype(logmessage) << ": " << message << std::endl;
	}

	std::string Logger::getLogtype(const logEnum& logmessage)
	{
		switch (logmessage)
		{
			case DEBUG:
				return "  DEBUG"; // Extra spaces for alignment
			case ERROR:
				return "  ERROR"; // Extra spaces for alignment
			case WARNING:
				return "WARNING";
			default:
				return "   INFO"; // Extra spaces for alignment
		}
	}

} /* namespace thywin */
