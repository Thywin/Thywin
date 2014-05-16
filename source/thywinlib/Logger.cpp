/*
 * Logger.cpp
 *
 *  Created on: 14 mei 2014
 *      Author: Erwin
 */

#include "Logger.h"
#include <ctime>

namespace thywin
{
	
	Logger::Logger(const std::string& logfileName)
	{
		logfileStream.open(logfileName.c_str(), std::ofstream::out | std::ofstream::app);
	}
	
	void Logger::log(const logEnum& logmessage, const std::string& message)
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
				return "DEBUG  "; // Extra space for alignment
			case ERROR:
				return "ERROR  "; // Extra space for alignment
			case WARNING:
				return "WARNING";
			default:
				return "INFO   "; // Extra space for alignment
		}
	}

} /* namespace thywin */
