/*
 * crawler.hpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas
 */

#ifndef CRAWLER_HPP_
#define CRAWLER_HPP_
#define MAX_CRAWL_LIST 10

namespace thywin
{
	class crawler
	{
	public:
		int crawl(char* url);
	private:
		void Parent(int* aiPY, char* url);
		void Child(int* aiPY, char* url);
	};
}
#endif /* CRAWLER_HPP_ */
