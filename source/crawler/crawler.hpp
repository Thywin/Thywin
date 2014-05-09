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
		void crawlUrl();
	private:
		int crawl(std::string url);
		void sendUrlDocument(std::string url, std::string documentName, int* aiPY);
		void Parent(int* aiPY, std::string url);
		void Child(int* aiPY, std::string url);
	};
}
#endif /* CRAWLER_HPP_ */
