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

			/**
			 * Returns a list of URIs extracted from the contents of a file.
			 * The content argument must contain the code of a page in it's entirety.
			 * The source argument contains the URI of the page where the content came from.
			 * The returned URI are constructed according to the source that is given with the content.
			 *
			 * @param	content	a string containing the code of a page
			 * @param	source	the URI of the page
			 * @return			the list of the URI found in the content
			 */
			std::vector<std::string> ExtractURIs(std::string content, std::string source);

			/**
			 *
			 */
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
