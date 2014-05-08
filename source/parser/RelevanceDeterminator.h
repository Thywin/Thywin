/*
 * RelevanceDeterminator.h
 *
 *  Created on: 8 mei 2014
 *      Author: Erwin
 */

#ifndef RELEVANCEDETERMINATOR_H_
#define RELEVANCEDETERMINATOR_H_

#include <map>
#include <string>

class RelevanceDeterminator {
public:
	RelevanceDeterminator();
	virtual ~RelevanceDeterminator();
	double DetermineRelevance(std::map<std::string, unsigned int> firstDocumentVector, std::map<std::string, unsigned int> secondDocumentVector);
};

#endif /* RELEVANCEDETERMINATOR_H_ */
