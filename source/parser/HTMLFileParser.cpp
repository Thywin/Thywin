/*
 * HTMLFileParser.cpp
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#include "HTMLFileParser.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

namespace thywin
{

HTMLFileParser::HTMLFileParser()
{
}

HTMLFileParser::~HTMLFileParser()
{
}

std::vector<std::string> HTMLFileParser::ExtractURIs(std::string content)
{
	return std::vector<std::string>();
}

std::string HTMLFileParser::ExtractText(std::string content)
{
	std::string text = std::string(content);

	unsigned int current = 0;
	int pointyBracketIndex = -1;
	while (current < text.size())
	{
		if (pointyBracketIndex == -1)
		{
			if (text.at(current) == '<')
			{
				pointyBracketIndex = current;
			}
		}
		else
		{
			if (text.at(current) == '>')
			{
				text.erase(pointyBracketIndex,
						(current + 1) - pointyBracketIndex);
				current = pointyBracketIndex;
				pointyBracketIndex = -1;
			}
		}
		current++;
	}

	return text;
}

} /* namespace thywin */
