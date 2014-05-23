/*
 * HTMLFileParser.cpp
 *
 *  Created on: May 8, 2014
 *      Author: Imre Woudstra
 *      Author: Thomas Gerritsen
 *      Author: Erwin Janssen
 */

#include "HTMLFileParser.h"

#include <sstream>

#include "Logger.h"

namespace thywin
{
	typedef FileParser::URIs URIs;

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

			uri = constructURI(uri, sourceURI);
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
		const std::string openBracket("<");
		const std::string closeBracket(">");

		std::stringstream textStream;
		std::string::size_type openBracketPosition = std::string::npos;
		std::string::size_type closeBracketPosition = std::string::npos;
		do
		{
			if(closeBracketPosition == std::string::npos)
			{
				openBracketPosition = content.find(openBracket, 0);
			}
			else
			{
				openBracketPosition = content.find(openBracket, closeBracketPosition);
			}

			if (openBracketPosition == std::string::npos)
			{
				textStream << content.substr(closeBracketPosition + closeBracket.length(), std::string::npos);
			}
			else
			{
				textStream
						<< content.substr(closeBracketPosition + closeBracket.length(),
								openBracketPosition - (closeBracketPosition + closeBracket.length()));
			}

			closeBracketPosition = content.find(closeBracket, openBracketPosition);
		} while (openBracketPosition != std::string::npos && closeBracketPosition != std::string::npos);

		return textStream.str();
	}

	std::string HTMLFileParser::constructURI(const std::string& rawURI, const std::string& sourceURI)
	{
		if (rawURI.length() == 0)
		{
			return rawURI;
		}

		std::string constructedURI;
		bool relativeURI = (getEndPositionOfProtocol(rawURI) == std::string::npos);

		if (relativeURI)
		{
			constructedURI = constructRelativeURI(rawURI, sourceURI);
		}
		else
		{
			constructedURI = rawURI;
		}

		const std::string fragmentIdentifier("#");
		return constructedURI.substr(0, constructedURI.find(fragmentIdentifier));
	}

	std::string HTMLFileParser::constructRelativeURI(const std::string& URI, const std::string& sourceURI)
	{
		const std::string relativeProtocolURI("//");
		const std::string relativeDirectoryUpURI("../");
		const std::string relativeHostURI("/");

		std::stringstream constructedURIStream;

		if (URI.compare(0, relativeProtocolURI.length(), relativeProtocolURI) == 0)
		{
			constructedURIStream << addProtocolToURI(URI);
		}
		else if (URI.compare(0, relativeDirectoryUpURI.length(), relativeDirectoryUpURI) == 0)
		{
			constructedURIStream
					<< constructRelativeURIWithDirectoryUp(URI, getHostPartOfURI(sourceURI),
							getPathPartOfURI(sourceURI));
		}
		else if (URI.compare(0, relativeHostURI.length(), relativeHostURI) == 0)
		{
			constructedURIStream << getHostPartOfURI(sourceURI) << URI;
		}
		else
		{
			const std::string colon(":");
			const std::string startQueryString("?");

			std::string::size_type colonPosition = URI.find(colon);
			std::string::size_type startQueryStringPosition = URI.find(startQueryString);

			if (colonPosition > startQueryStringPosition || colonPosition == std::string::npos)
			{
				constructedURIStream << getHostPartOfURI(sourceURI) << getPathPartOfURI(sourceURI) << URI;
			}
		}

		return constructedURIStream.str();
	}

	std::string HTMLFileParser::constructRelativeURIWithDirectoryUp(const std::string& inputURI,
			const std::string& host, const std::string& path)
	{
		std::string::size_type pathEndPosition = path.length() - 1;
		std::string::size_type inputURIStartPosition = 0;

		const std::string relativeDirectoryUpURI("../");
		const std::string slash("/");
		while (inputURI.compare(inputURIStartPosition, relativeDirectoryUpURI.length(), relativeDirectoryUpURI) == 0)
		{
			inputURIStartPosition += relativeDirectoryUpURI.length();
			pathEndPosition = path.find_last_of(slash, pathEndPosition - slash.length());
			if (pathEndPosition == std::string::npos)
			{
				pathEndPosition = 0;
				break;
			}
		}

		return host + path.substr(0, pathEndPosition + slash.length())
				+ inputURI.substr(inputURIStartPosition, inputURI.length() - inputURIStartPosition);
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
		const std::string relativeProtocolURI("//");
		if (URI.compare(0, relativeProtocolURI.length(), relativeProtocolURI) == 0)
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
