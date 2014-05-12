/*
 * HTMLFileParser.h
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 *      Author: Thomas Gerritsen
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

			std::vector<std::string> ExtractURIs(std::string content, std::string source);
			std::string ExtractText(std::string content);
		private:
			std::vector<std::string> parseString(std::string input, std::string::size_type endHeadPos,
					std::string source);
			std::string constructURI(std::string input, std::string host, std::string path);
			std::vector<std::string> splitSource(std::string source);
			std::string removeOneUps(int amount, std::string path);
			std::string formHost(std::string source);
			std::string formPath(std::string source, int hostLength);
	};

} /* namespace thywin */

#endif /* HTMLFILEPARSER_H_ */
