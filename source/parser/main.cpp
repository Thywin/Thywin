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
		if (argc != 3)
		{
			std::cout << "Usage: " << argv[0] << " [ipaddress] [port]" << std::endl;
			return EXIT_FAILURE;
		}

		Parser parser(argv[1], std::stoi(argv[2]));
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
