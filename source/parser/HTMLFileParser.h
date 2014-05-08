/*
 * HTMLFileParser.h
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#ifndef HTMLFILEPARSER_H_
#define HTMLFILEPARSER_H_

#include <string>
#include <vector>

namespace thywin
{

class HTMLFileParser
{
public:
	HTMLFileParser();
	virtual ~HTMLFileParser();

	std::vector<std::string> ExtractURIs(std::string content);
	std::string ExtractText(std::string content);
};

} /* namespace thywin */

#endif /* HTMLFILEPARSER_H_ */
