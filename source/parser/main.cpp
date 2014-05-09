#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "DocumentVector.h"
using namespace thywin;

int main(int argc, char** argv)
{
	DocumentVector a = DocumentVector("aapje aapje aapje aapje aapje hallo hallo hallo hallo");
	DocumentVector b = DocumentVector("aapje aapje doei doei doei doei doei doei");

	std::cout << a.CalculateSimilarity(&b) << std::endl;

	return EXIT_SUCCESS;
}
