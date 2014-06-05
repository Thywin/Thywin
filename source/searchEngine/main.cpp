/*
 * main.cpp
 *
 *  Created on: 27 mei 2014
 *      Author: Erwin Janssen
 */

#include <algorithm>
#include <iostream>
#include <Logger.h>
#include <stdlib.h>
#include <stdexcept>
#include "SearchEngineServer.h"

using namespace thywin;

int main(int argc, char** argv)
{
	Logger logger("SearchEngine.log");
	try
	{
		if (argc != 3)
		{
			std::cerr
					<< "Incorrect useage of this program. Correct usage is:\nsearchEngine <port number> <maxium concurrent connections>"
					<< std::endl;
			logger.Log(ERROR, "No port and number of clients has been given.");
			return EXIT_FAILURE;
		}
		else
		{
			SearchEngineServer server(std::stoi(argv[1]), std::stoi(argv[2]));
			return EXIT_SUCCESS;
		}
	}
	catch (std::invalid_argument& e)
	{
		logger.Log(ERROR, "Invalid argument exception: " + std::string(e.what()));
		return EXIT_FAILURE;
	}
	catch (std::exception& e)
	{
		logger.Log(ERROR, "Catched an exception: " + std::string(e.what()));
		return EXIT_FAILURE;
	}
	catch (...)
	{
		logger.Log(ERROR, "Unknown exception occurred");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
