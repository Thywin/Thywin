/*
 * HTMLFileParser.h
 *
 *  Created on: May 8, 2014
 *      Author: Imre Woudstra
 *      Author: Thomas Gerritsen
 *      Author: Erwin Janssen
 */

#ifndef HTMLFILEPARSER_H_
#define HTMLFILEPARSER_H_

#include <string>
#include <vector>
#include "FileParser.h"

namespace thywin
{
	/**
	 * File parser for HTML files
	 * Can be used to extract URIs from a HTML file
	 * Can be used to extract text from a HTML file
	 */
	class HTMLFileParser: public FileParser
	{
		public:
			/**
			 * Default destructor
			 */
			virtual ~HTMLFileParser();

			/**
			 * @see FileParser.ExtractURIs() 
			 */
			virtual URIs ExtractURIs(const std::string& content, const std::string& sourceURI);

			/**
			 * @see FileParser.ExtractText()
			 */
			virtual std::string ExtractText(const std::string& content);
		private:
			std::string constructURI(const std::string& inputURI, const std::string& sourceURI);
			std::string constructRelativeURI(const std::string& input, const std::string& sourceURI);
			std::string constructRelativeURIWithDirectoryUp(const std::string& URI, const std::string& host,
					const std::string& path);
			std::string getHostPartOfURI(const std::string& source);
			std::string getPathPartOfURI(const std::string& source);
			std::string addProtocolToURI(const std::string& source);
			std::string::size_type getEndPositionOfProtocol(const std::string& URI);
			std::string::size_type getPositionOfFirstSlashAfterProtocol(const std::string& URI);
	};

} /* namespace thywin */

#endif /* HTMLFILEPARSER_H_ */
