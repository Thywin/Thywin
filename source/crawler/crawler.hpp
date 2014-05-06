/*
 * crawler.hpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas
 */

#ifndef CRAWLER_HPP_
#define CRAWLER_HPP_
#define MAX_CRAWL_LIST 10

namespace crawler
{
	int crawl(char* url);
	void Parent(int* aiPX, int* aiPY, char* url);
	void Child(int* aiPX, int* aiPY, char* url);
}
#endif /* CRAWLER_HPP_ */
