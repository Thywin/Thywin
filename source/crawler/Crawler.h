/*
 * Crawler.hpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas Kooi, Bobby Bouwmann
 */

#ifndef CRAWLER_HPP_
#define CRAWLER_HPP_
#define MAX_CRAWL_LIST 10

#include <string>
#include "Communicator.h"
#include "Logger.h"

namespace thywin
{
	const short NOFLAG = 0;

	class Crawler
	{
		public:
			/**
			 * Constructor of the crawler. The constructor creates a Crawler object and sets the ipaddress for
			 * the socket.
			 */
			Crawler(const std::string& ipaddress, const int& port);

			/**
			 * This function gets an URI from the master server. It then starts to crawl the URI and sends
			 * the crawled data and the URI back to the master.
			 */
			void CrawlURI();
		private:
			Communicator communication;
			int crawl(const std::string& URI);
			void sendURIDocument(int* pagePipe, const std::string& URI);
			void startCurl(int* pagePipe, const std::string& URI);
			Logger logger;
	};
}

#endif /* CRAWLER_HPP_ */
