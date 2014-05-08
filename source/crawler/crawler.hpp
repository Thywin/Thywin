/*
 * crawler.hpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas, Bobby
 */

#ifndef CRAWLER_HPP_
#define CRAWLER_HPP_
#define MAX_CRAWL_LIST 10

#include <sstream>

namespace thywin
{
	const short URL = 1;
	const short DOCUMENT = 2;
	const short GET = 1;
	const short PUT = 2;

	struct requestContainer {
		short type;
		short action;
		int size;
	};

	class crawler
	{
	public:
		int crawl(char* url);
		char* getUrl();
		void sendUrlDocument(char* url, const char* documentName);
	private:
		void Parent(int* aiPY, char* url);
		void Child(int* aiPY, char* url);
	};
}
#endif /* CRAWLER_HPP_ */
