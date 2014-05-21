#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Parser.h"
#include "Logger.h"

using namespace thywin;

int main(int argc, char** argv)
{
	Logger logger("parser.log");
	try
	{
		Parser parser("192.168.100.13", "192.168.100.13", "192.168.100.13");
		parser.Run();

		return EXIT_SUCCESS;
	}
	catch (std::exception& e)
	{
		logger.Log(ERROR, "Parser: " + std::string(e.what()));
	}
	catch (...)
	{
		logger.Log(ERROR, "Unexpected error while parsing");
	}
}
