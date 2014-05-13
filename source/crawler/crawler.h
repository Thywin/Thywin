/*
 * crawler.hpp
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
			 *
			 */
			void CrawlUri();
		private:
			std::string ipaddress;
			int crawl(std::string uri);
			void sendUriDocument(int* wgetCommunicationPipe, std::string uri);
			void startWget(int* wgetCommunicationPipe, std::string uri);
	};
}
#endif /* CRAWLER_HPP_ */
