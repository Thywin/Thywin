/*
 * HTMLFileParser.cpp
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 *      Author: Thomas Gerritsen
 */

#include "HTMLFileParser.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <string.h>
using namespace std;

namespace thywin
{

	HTMLFileParser::HTMLFileParser()
	{
	}

	HTMLFileParser::~HTMLFileParser()
	{
	}

	std::vector<std::string> HTMLFileParser::ExtractURIs(std::string content, std::string source)
	{
		std::string stringToParse = content;
		std::vector<string> list;
		std::vector<string> buffer;

		std::string::size_type endHeadFound = stringToParse.find("</head>");
		if (endHeadFound == string::npos)
		{
			endHeadFound = 0;
		}

		buffer = parseString(stringToParse, endHeadFound, source);
		for (unsigned int i = 0; i < buffer.size(); i++)
		{
			list.push_back(buffer.at(i));
		}

		std::sort(list.begin(), list.end());
		std::vector<std::string>::iterator it = std::unique(list.begin(), list.end());
		list.resize(std::distance(list.begin(), it));

		return list;
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
					text.erase(pointyBracketIndex, (current + 1) - pointyBracketIndex);
					current = pointyBracketIndex;
					pointyBracketIndex = -1;
				}
			}
			current++;
		}

		return text;
	}

	std::vector<std::string> HTMLFileParser::parseString(string input, std::string::size_type endHeadPos,
			std::string source)
	{
		std::vector<std::string> hostPath = splitSource(source);
		std::string href("href=\"");
		std::string character("\"");
		std::string line = input;
		std::vector<string> list;
		unsigned int startPos = endHeadPos;

		while (startPos < line.length())
		{
			std::string::size_type pos = line.find(href, startPos);
			if (pos == string::npos)
			{
				break;
			}

			int hrefPos = pos + href.length();

			std::string::size_type quotePos = line.find(character, hrefPos);
			if (quotePos == string::npos)
			{
				break;
			}

			int length = quotePos - hrefPos;

			std::string uri = line.substr(hrefPos, length);
			startPos = quotePos;
			if (!uri.empty())
			{
				uri = constructURI(uri, hostPath.at(0), hostPath.at(1));
				if (!uri.empty())
				{
					list.push_back(uri);
				}
			}
		}
		return list;
	}

	std::string HTMLFileParser::constructURI(std::string input, std::string host, std::string path)
	{
		std::vector<const char *> bannedTags;
		const char *http("http://");
		const char *https("https://");
		const char *www("www");
		bannedTags.push_back("://");
		bannedTags.push_back("mailto:");
		bannedTags.push_back("file:");
		bannedTags.push_back("javascript:");

		std::string::size_type httpPos = input.find(http, 0, strlen(http));
		std::string::size_type httpsPos = input.find(https, 0, strlen(https));
		std::string::size_type wwwPos = input.find(www, 0, strlen(www));
		if (httpPos == string::npos && httpsPos == string::npos && wwwPos == string::npos
				&& !(input[0] == '/' && input[1] == '/'))
		{
			for (unsigned int i = 0; i < bannedTags.size(); i++)
			{
				std::string::size_type pos = input.find(bannedTags.at(i));
				if (pos != string::npos)
				{
					return "";
				}
			}
			if (input[0] == '#')
			{
				return "";
			}

			if (input[0] == '/')
			{
				return host + input;
			}

			int oneUpCounter = 0;
			std::string::size_type foundOneUp = 0;
			do
			{
				foundOneUp = input.find("../", foundOneUp);
				if (foundOneUp != string::npos)
				{
					input.erase(0, 3);
					oneUpCounter++;
				}
			} while (foundOneUp != string::npos);

			std::string uriPath = removeOneUps(oneUpCounter, path);

			if (oneUpCounter > 0)
			{
				return host + uriPath + input;
			}
			else
			{
				return host + "/" + input;
			}
		}
		return input;
	}

	std::string HTMLFileParser::removeOneUps(int amount, std::string path)
	{
		std::string uriPath = path;
		for (int i = 0; i < amount; i++)
		{
			std::string::size_type lastFoundSlashPos = uriPath.find("/");
			if (lastFoundSlashPos == string::npos)
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
				if (newSlashPos != string::npos)
				{
					secondToLastSlashPos = lastFoundSlashPos;
					lastFoundSlashPos = newSlashPos;
				}
			} while (newSlashPos != string::npos);

			if (secondToLastSlashPos != string::npos)
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

	std::string HTMLFileParser::formHost(std::string source)
	{
		std::string host = source;
		std::string::size_type lastFoundDotPos = host.find(".");
		if (lastFoundDotPos == string::npos)
		{
			return "";
		}
		std::string::size_type newDotPos;
		std::string::size_type secondToLastDotPos;
		do
		{
			newDotPos = host.find(".", lastFoundDotPos + 1);
			if (newDotPos != string::npos)
			{
				secondToLastDotPos = lastFoundDotPos;
				lastFoundDotPos = newDotPos;
			}
		} while (newDotPos != string::npos);

		std::string::size_type checkHtm = host.find("htm", lastFoundDotPos);
		if (checkHtm != string::npos)
		{
			lastFoundDotPos = secondToLastDotPos;
		}
		std::string::size_type pathPos = host.find("/", lastFoundDotPos);
		if (pathPos != string::npos)
		{
			host = host.erase(pathPos, host.length() - pathPos);
		}
		return host;
	}

	std::string HTMLFileParser::formPath(std::string source, int hostLength)
	{
		std::string path = source;
		std::string::size_type htmPos = path.find(".htm");
		if (htmPos != string::npos)
		{
			std::string::size_type lastFoundSlashPos = path.find("/");
			std::string::size_type newSlashPos;
			std::string::size_type secondToLastSlashPos;
			do
			{
				newSlashPos = path.find("/", lastFoundSlashPos + 1);
				if (newSlashPos != string::npos)
				{
					secondToLastSlashPos = lastFoundSlashPos;
					lastFoundSlashPos = newSlashPos;
				}
			} while (newSlashPos != string::npos);

			if (path[path.length() - 1] == '/')
			{
				lastFoundSlashPos = secondToLastSlashPos;
			}
			path = path.erase(lastFoundSlashPos, path.length() - lastFoundSlashPos);
		}
		else
		{
			if (path[path.length() - 1] == '/')
			{
				path = path.erase(path.length() - 1, 1);
			}
		}
		path = path.erase(0, hostLength);
		return path;
	}

	std::vector<std::string> HTMLFileParser::splitSource(std::string source)
	{
		std::string::size_type containsNoHttp = source.find("http://");
		std::string::size_type containsNoHttps = source.find("https://");
		if (containsNoHttp == string::npos && containsNoHttps == string::npos)
		{
			if (!(source[0] == '/' && source[1] == '/'))
			{
				source = "http://" + source;
			}
			else
			{
				source = "http:" + source;
			}
		}
		std::vector<std::string> splitSource;
		std::string host = formHost(source);
		splitSource.push_back(host);
		std::string path = formPath(source, host.length());
		splitSource.push_back(path);
		return splitSource;
	}

} /* namespace thywin */
