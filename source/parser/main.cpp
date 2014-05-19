#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Parser.h"

int main(int argc, char** argv)
{
	thywin::Parser parser("192.168.100.13", "192.168.100.13", "192.168.100.13");
	parser.Run();

	return EXIT_SUCCESS;
}
