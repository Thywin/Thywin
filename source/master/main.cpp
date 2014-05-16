/*
 * main.cpp
 *
 *  Created on: 13 mei 2014
 *      Author: Thomas
 */

#include <stdio.h>
#include <stdlib.h>
#include "Server.h"

int main()
{
	thywin::Server srv;
	srv.SetUp(7500);
	srv.Listen();
	printf("Server shutting down\n");
	return(EXIT_SUCCESS);
}

