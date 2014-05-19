/*
 * main.cpp
 *
 *  Created on: 13 mei 2014
 *      Author: Thomas Kooi
 */

#include <stdio.h>
#include <stdlib.h>
#include "Server.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("No Port given");
		return(EXIT_FAILURE);
	} else {
		thywin::Master::InitializeMaster();
		thywin::Server srv;
		const int portNumber = atoi(argv[1]);
		srv.SetUp(portNumber);
		srv.Listen();
		printf("Server shutting down\n");
		thywin::Master::DBConnection.Disconnect();
		return(EXIT_SUCCESS);
	}
}

