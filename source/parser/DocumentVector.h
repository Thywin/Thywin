/*
 * DocumentVector.h
 *
 *  Created on: 8 mei 2014
 *      Author: Erwin
 */

#ifndef DOCUMENTVECTOR_H_
#define DOCUMENTVECTOR_H_

#include <map>
#include <string>

namespace thywin {

class DocumentVector: public std::map<std::string, unsigned int> {
public:
	DocumentVector();
	DocumentVector(std::string text);
	virtual ~DocumentVector();
	double CompareTo(DocumentVector* documentVector);
};

} /* namespace thywin */

#endif /* DOCUMENTVECTOR_H_ */
