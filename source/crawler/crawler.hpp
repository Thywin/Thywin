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
	class URL
	{
		public:
			char* getURL();
		private:
			char* url;
	};

	class Parser
	{
		public:
			char *getIP();
		private:
			char *ip;
	};

	class Scheduler
	{
		public:
			char *getIP();
		private:
			char *ip;
	};

	class Crawler
	{
		public:

		protected:

		private:
			URL crawlList[MAX_CRAWL_LIST];
			void findParser();
			void findScheduler();
			void requestNextURL();
			void crawlNext();
	};
}
#endif /* CRAWLER_HPP_ */
