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
	
	Logger::Logger(std::string logfileName)
	{
		logfileStream.open(logfileName.c_str(), std::ofstream::out | std::ofstream::app);
	}
	
	Logger::~Logger()
	{
		// TODO Auto-generated destructor stub
	}
	
	void Logger::log(logEnum logmessage, std::string message)
	{
		unsigned int buffersize = 50;
		char timeString[buffersize];
		time_t currentTime = time(NULL);
		struct tm* timeinfo = localtime(&currentTime);
		strftime(timeString, buffersize, "%F %T", timeinfo);
		
		logfileStream << "[" << timeString << "]" << " " << getLogtype(logmessage) << ": " << message << std::endl;
	}

	std::string Logger::getLogtype(logEnum logmessage)
	{
		switch (logmessage)
		{
			case DEBUG:
				return "DEBUG";
			case ERROR:
				return "ERROR";
			default:
				return "INFO ";
		}
	}

} /* namespace thywin */
