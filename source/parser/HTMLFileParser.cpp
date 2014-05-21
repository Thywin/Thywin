/*
 * HTMLFileParser.cpp
 *
 *  Created on: May 8, 2014
 *      Author: Imre Woudstra
 *      Author: Thomas Gerritsen
 *      Author: Erwin Janssen
 */

#include "HTMLFileParser.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <string.h>

#include "Logger.h"

namespace thywin
{
	typedef FileParser::URIs URIs;
	Logger logger("uris.log");

	HTMLFileParser::~HTMLFileParser()
	{
	}

	URIs HTMLFileParser::ExtractURIs(const std::string& content, const std::string& sourceURI)
	{
		std::string::size_type endHTMLHeadPosition = content.find("</head>");
		if (endHTMLHeadPosition == std::string::npos)
		{
			endHTMLHeadPosition = 0;
		}

		const std::string href("href=\"");
		const std::string quoteCharacter("\"");
		URIs extractedURIs;

		std::string hostPartOfSourceURI = getHostPartOfURI(sourceURI);
		std::string pathPartOfSourceURI = getPathPartOfURI(sourceURI);

		std::string::size_type currentParsePosition = endHTMLHeadPosition;
		while (currentParsePosition < content.length())
		{
			std::string::size_type hrefPosition = content.find(href, currentParsePosition);
			if (hrefPosition == std::string::npos)
			{
				break;
			}
			std::string::size_type startURIPosition = hrefPosition + href.length();

			std::string::size_type endURIPosition = content.find(quoteCharacter, startURIPosition);
			if (endURIPosition == std::string::npos)
			{
				break;
			}

			std::string uri = content.substr(startURIPosition, endURIPosition - startURIPosition);
			logger.Log(INFO, uri);

			uri = constructURI(uri, hostPartOfSourceURI, pathPartOfSourceURI);
			if (!uri.empty())
			{
				extractedURIs.push_back(uri);
			}

			currentParsePosition = endURIPosition;
		}

		return extractedURIs;
	}

	std::string HTMLFileParser::ExtractText(const std::string& content)
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
					text.erase(pointyBracketIndex, (current + 1) - pointyBracketIndex);
					current = pointyBracketIndex;
					pointyBracketIndex = -1;
				}
			}
			current++;
		}

		return text;
	}

	std::string HTMLFileParser::constructURI(const std::string& rawURI, const std::string& host,
			const std::string& path)
	{
		if (rawURI.length() == 0)
		{
			return rawURI;
		}

		bool relativeURI = (getEndPositionOfProtocol(rawURI) == std::string::npos);
		if (relativeURI)
		{
			return constructRelativeURI(rawURI, host, path);
		}
		else
		{
			return rawURI;
		}
		/*
		 std::string tempInput = std::string(rawURI);
		 std::vector<const char *> bannedTags;
		 const char *http("http://");
		 const char *https("https://");
		 const char *www("www");
		 bannedTags.push_back("mailto:");
		 bannedTags.push_back("file:");
		 bannedTags.push_back("javascript:");

		 std::string::size_type httpPos = tempInput.find(http, 0, strlen(http));
		 std::string::size_type httpsPos = tempInput.find(https, 0, strlen(https));
		 std::string::size_type wwwPos = tempInput.find(www, 0, strlen(www));
		 if ((httpPos == std::string::npos || httpPos != 0) && (httpsPos == std::string::npos || httpsPos != 0)
		 && (wwwPos == std::string::npos || wwwPos != 0))
		 {
		 for (unsigned int i = 0; i < bannedTags.size(); i++)
		 {
		 std::string::size_type pos = tempInput.find(bannedTags.at(i));
		 if (pos != std::string::npos)
		 {
		 return std::string();
		 }
		 }
		 if (tempInput[0] == '#')
		 {
		 return std::string();
		 }

		 if (tempInput[0] == '/')
		 {
		 return host + tempInput;
		 }

		 int oneUpCounter = 0;
		 std::string::size_type foundOneUp = 0;
		 do
		 {
		 foundOneUp = tempInput.find("../", foundOneUp);
		 if (foundOneUp != std::string::npos)
		 {
		 tempInput.erase(0, 3);
		 oneUpCounter++;
		 }
		 } while (foundOneUp != std::string::npos);

		 std::string uriPath = removeOneUps(oneUpCounter, path);

		 if (oneUpCounter > 0)
		 {
		 return host + uriPath + tempInput;
		 }
		 else
		 {
		 return host + "/" + tempInput;
		 }
		 }
		 return tempInput;
		 */
	}

	std::string HTMLFileParser::constructRelativeURI(const std::string& URI, const std::string& host,
			const std::string& path)
	{
		const std::string relativeProtocolURI("//");
		const std::string relativeDirectoryUpURI("../");
		const std::string relativeHostURI("/");

		if (URI.compare(0, relativeProtocolURI.length(), relativeProtocolURI) == 0)
		{
			return addProtocolToURI(URI);
		}
		else if (URI.compare(0, relativeDirectoryUpURI.length(), relativeDirectoryUpURI) == 0)
		{
			return constructRelativeURIWithDirectoryUp(URI, host, path);
		}
		else if (URI.compare(0, relativeHostURI.length(), relativeHostURI) == 0)
		{
			return host + URI;
		}
		else
		{
			return host + path + URI;
		}
	}

	std::string HTMLFileParser::constructRelativeURIWithDirectoryUp(const std::string& URI, const std::string& host,
			const std::string& path)
	{
		std::string::size_type pathEndPosition = path.length() - 1;
		std::string::size_type URIStartPosition = 0;

		const std::string relativeDirectoryUpURI("../");
		const std::string slash("/");
		while (URI.compare(URIStartPosition, relativeDirectoryUpURI.length(), relativeDirectoryUpURI) == 0)
		{
			URIStartPosition += relativeDirectoryUpURI.length();
			pathEndPosition = path.find_last_of(slash, pathEndPosition - 1);
			if (pathEndPosition == std::string::npos)
			{
				pathEndPosition = 0;
				break;
			}
		}

		return host + path.substr(0, pathEndPosition + 1)
				+ URI.substr(URIStartPosition, URI.length() - URIStartPosition);
	}

	std::string HTMLFileParser::removeOneUps(const int amount, const std::string& path)
	{
		std::string uriPath = path;
		for (int i = 0; i < amount; i++)
		{
			std::string::size_type lastFoundSlashPos = uriPath.find("/");
			if (lastFoundSlashPos == std::string::npos)
			{
				if (uriPath[uriPath.length() - 1] != '/')
				{
					uriPath += '/';
				}
				break;
			}
			std::string::size_type newSlashPos;
			std::string::size_type secondToLastSlashPos;
			do
			{
				newSlashPos = uriPath.find("/", lastFoundSlashPos + 1);
				if (newSlashPos != std::string::npos)
				{
					secondToLastSlashPos = lastFoundSlashPos;
					lastFoundSlashPos = newSlashPos;
				}
			} while (newSlashPos != std::string::npos);

			if (secondToLastSlashPos != std::string::npos)
			{
				if (lastFoundSlashPos == uriPath.length() - 1)
				{
					if ((lastFoundSlashPos - secondToLastSlashPos) >= 2)
					{
						uriPath = uriPath.erase(secondToLastSlashPos, lastFoundSlashPos - secondToLastSlashPos);
					}
				}
				else
				{
					uriPath = uriPath.erase(lastFoundSlashPos, uriPath.length() - lastFoundSlashPos);
				}
				if (uriPath[uriPath.length() - 1] != '/')
				{
					uriPath += '/';
				}
			}
		}
		return uriPath;
	}

	std::string HTMLFileParser::getHostPartOfURI(const std::string& sourceURI)
	{
		std::string sourceURIWithProtocol = addProtocolToURI(sourceURI);
		std::string::size_type firstNonProtocolSlashPosition = getPositionOfFirstSlashAfterProtocol(
				sourceURIWithProtocol);

		if (firstNonProtocolSlashPosition == std::string::npos)
		{
			return sourceURI;
		}
		else
		{
			return sourceURI.substr(0, firstNonProtocolSlashPosition);
		}
	}

	std::string HTMLFileParser::getPathPartOfURI(const std::string& sourceURI)
	{
		std::string sourceURIWithProtocol = addProtocolToURI(sourceURI);

		std::string::size_type firstNonProtocolSlashPosition = getPositionOfFirstSlashAfterProtocol(
				sourceURIWithProtocol);

		if (firstNonProtocolSlashPosition == std::string::npos)
		{
			return std::string("/");
		}
		else
		{
			std::string::size_type firstHashtagPosition = sourceURIWithProtocol.find("#");
			std::string::size_type firstQuestionMarkPosition = sourceURIWithProtocol.find("?");
			std::string::size_type pathBoundary;
			if (firstHashtagPosition == std::string::npos && firstQuestionMarkPosition != std::string::npos)
			{
				pathBoundary = firstQuestionMarkPosition;
			}
			else if (firstHashtagPosition != std::string::npos && firstQuestionMarkPosition == std::string::npos)
			{
				pathBoundary = firstHashtagPosition;
			}
			else
			{
				pathBoundary = std::min(firstHashtagPosition, firstQuestionMarkPosition);
			}

			std::string::size_type endPathPosition = sourceURIWithProtocol.find_last_of("/", pathBoundary);
			return sourceURIWithProtocol.substr(firstNonProtocolSlashPosition,
					(endPathPosition + 1) - firstNonProtocolSlashPosition);
		}
	}

	std::string HTMLFileParser::addProtocolToURI(const std::string& URI)
	{
		if (URI.compare(0, 2, "//") == 0)
		{
			return "http:" + URI;
		}
		else if (getEndPositionOfProtocol(URI) == std::string::npos)
		{
			return "http://" + URI;
		}
		else
		{
			return URI;
		}
	}

	std::string::size_type HTMLFileParser::getEndPositionOfProtocol(const std::string& URI)
	{
		const std::string protocolEnd("://");
		std::string::size_type protocolEndPosition = URI.find(protocolEnd);
		if (protocolEndPosition != std::string::npos)
		{
			protocolEndPosition += protocolEnd.length();
		}
		return protocolEndPosition;
	}

	std::string::size_type HTMLFileParser::getPositionOfFirstSlashAfterProtocol(const std::string& URI)
	{
		return URI.find("/", getEndPositionOfProtocol(URI));
	}

} /* namespace thywin */
