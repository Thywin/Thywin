#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "HTMLFileParser.h"

int main(int argc, char** argv)
{
	std::string temp = std::string("dit is een <kaas> test <string> haha\n");
	thywin::HTMLFileParser parser;

	std::string newstr = parser.ExtractText(temp);

	std::cout << temp;
	std::cout << newstr;

	return EXIT_SUCCESS;
}
