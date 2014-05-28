/*
 * SearchEngine.h
 *
 *  Created on: 28 mei 2014
 *      Author: Erwin
 */

#ifndef SEARCHENGINE_H_
#define SEARCHENGINE_H_

#include "MultiURIPacket.h"
#include <string>

namespace thywin
{
	
	class SearchEngine
	{
		public:
			SearchEngine();
			SearchEngine(const SearchEngine& searchEngine);
			virtual ~SearchEngine();
			
			MultiURIPacket Search(std::string searchWords);
		private:
			static bool compareURIsWithRelevance(std::pair<std::string, double> first, std::pair<std::string, double> second);
	};

} /* namespace thywin */

#endif /* SEARCHENGINE_H_ */
