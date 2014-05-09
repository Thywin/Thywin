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

	/**
	 * This constructor takes a string as argument.
	 * Converts the input string to lowercase, counts the words and creates a DocumentVector
	 */
	DocumentVector(std::string text);

	virtual ~DocumentVector();

	/**
	 * Computes the cosine similairy between the DocumentVector it is called upon
	 * and the DocumentVector passed as argument.
	 */
	double CalculateSimilarity(DocumentVector* documentVector);

	/**
	 * Computes the magnitude (length) of the DocumentVector
	 */
	double GetMagnitude();

	/**
	 * Computes the dot product between the DocumentVector it is called upon
	 * and the DocumentVector passed as argument.
	 */
	double DotProduct(DocumentVector* documentVector);
};

} /* namespace thywin */

#endif /* DOCUMENTVECTOR_H_ */
