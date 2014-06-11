/*
 * FileParser.h
 *
 *  Created on: May 20, 2014
 *      Author: Imre Woudstra
 *      Author: Erwin Janssen
 */

#ifndef FILEPARSER_H_
#define FILEPARSER_H_

namespace thywin
{
	/**
	 * Abstract FileParser to be implemented for specific file types.
	 * Derived classes will be able to extract URI's and Text from the content
	 */
	class FileParser
	{
		public:
			typedef std::vector<std::string> URIs;
			/**
			 * Default destructor
			 */
			virtual ~FileParser()
			{
			}

			/**
			 * Returns a list of URIs extracted from the contents of a file.
			 *
			 * @param content is a string that contain the code of a file in it's entirety.
			 * @param sourceURI the URI of the file passed as content.
			 * @return a vector of all the URI's found in the content. All URI's found in the 
			 * content will be formed to an absolute URI
			 */
			virtual URIs ExtractURIs(const std::string& content, const std::string& sourceURI) = 0;

			/**
			 * Returns the actual text content of a file.
			 * Based on the type of content all non relevant text will be removed.
			 * 
			 * @param content a string containing the content of a file.
			 * @return the text that has been extracted from the content
			 */
			virtual std::string ExtractText(const std::string& content) = 0;
	};

} /* namespace thywin */

#endif /* FILEPARSER_H_ */
