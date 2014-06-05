/*
 * main.cpp
 *
 *  Created on: 13 mei 2014
 *      Author: Thomas Kooi
 */

#include <stdio.h>
#include <stdlib.h>
#include "Server.h"
#include <stdexcept>
#include "Master.h"
#include "Logger.h"

using namespace thywin;

int main(int argc, char* argv[])
{
	Logger logger("Master.log");
	try
	{
		if (argc != 2)
		{
			std::cout << "Usage: " << argv[0] << "[port]" << std::endl;
			return EXIT_FAILURE;
		}
		else
		{
			const int portNumber = std::stoi(argv[1]);
			logger.Log(INFO, "Starting Master.");
			Master::InitializeMaster();
			Server srv(portNumber);
			logger.Log(INFO, "Server shutting down.");
			return (EXIT_SUCCESS);
		}
	}
	catch (std::invalid_argument& e)
	{
		logger.Log(ERROR, "Invalid argument exception: " + std::string(e.what()));
	}
	catch (std::exception& e)
	{
		logger.Log(ERROR, "Catched an exception on main: " + std::string(e.what()));
	}
	catch (...)
	{
		logger.Log(ERROR, "Something went terribly wrong!");
	}
	return (EXIT_SUCCESS);
}

