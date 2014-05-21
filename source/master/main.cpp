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

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			printf("No port has been given..\n");
			return (EXIT_FAILURE);
		}
		else
		{
			thywin::Master::InitializeMaster();
			const int portNumber = atoi(argv[1]);
			thywin::Server srv(portNumber);
			printf("Server shutting down\n");
			return (EXIT_SUCCESS);
		}
	}
	catch (std::exception& e)
	{
		printf("Catched an exception on main: %s\n",e.what());
	}
}

