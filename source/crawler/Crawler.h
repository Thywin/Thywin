/*
 * Crawler.hpp
 *
 *  Created on: 25 apr. 2014
 *      Author: Thomas, Bobby
 */

#ifndef CRAWLER_HPP_
#define CRAWLER_HPP_
#define MAX_CRAWL_LIST 10

#include <string>

namespace thywin
{
	const short URI = 1;
	const short DOCUMENT = 2;
	const short GET = 1;
	const short PUT = 2;
	const short NOFLAG = 0;

	struct ThywinPacket
	{
			short type;
			short action;
			int size;
	};

	class Crawler
	{

		public:
			/**
			 * Constructor of the crawler. The constructor creates a Crawler object and sets the ipaddress for
			 * the socket.
			 */
			Crawler(std::string ipaddress);

			/**
			 * This function gets an URI from the master server. It then starts to crawl the URI and sends
			 * the crawled data and the URI back to the master. Then the process starts all over.
			 */
			void CrawlURI();
		private:
			std::string ipaddress;
			int crawl(std::string URI);
			void sendURIDocument(int* wgetCommunicationPipe, std::string URI);
			void startWget(int* wgetCommunicationPipe, std::string URI);
	};
}
#endif /* CRAWLER_HPP_ */
