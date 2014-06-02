/*
 * DocumentVector.h
 *
 *  Created on: 8 mei 2014
 *      Author: Erwin Janssen
 */

#ifndef DOCUMENTVECTOR_H_
#define DOCUMENTVECTOR_H_

#include <map>
#include <string>
#include "Communicator.h"
#include "ThywinPacketContent.h"

namespace thywin
{
	
	class DocumentVector: public ThywinPacketContent, public std::map<std::string, unsigned int>
	{
		public:
			
			/**
			 * Default constructor
			 * Use this to create an empty DocumentVector and manually fill it later.
			 */
			DocumentVector();

			/**
			 * This constructor takes a string as argument.
			 * Creates a DocumentVector based on the input string. Sets the text to lower case,
			 * then counts the words and creates a DocumentVector based on the number of occurrences of every word.
			 * @param text The input text of which a DocumentVector must be created.
			 */
			DocumentVector(std::string text);

			/**
			 * Default destructor
			 */
			virtual ~DocumentVector();

			/**
			 * Computes the cosine similarity between the DocumentVector it is called upon
			 * and the DocumentVector passed as argument.
			 * @param documentVector The DocumentVector that will be compared.
			 * @return The similarity between the two DocumentVectors, a double with a value between 0 and 1 (both inclusive)
			 */
			double CalculateSimilarity(DocumentVector& documentVector);

			/**
			 * Computes the magnitude (length) of this DocumentVector.
			 * @return The magnitude (length) of this DocumentVector, this value will be 0 or greater.
			 */
			double GetMagnitude();

			/**
			 * Computes the dot product between the DocumentVector it is called upon and the DocumentVector passed as argument.
			 * @return The dot product between the two DocumentVectors, this value will be 0 or greater.
			 */
			double DotProduct(DocumentVector& documentVector);
			
			/**
			 * @see ThywinPacketContent.Serialize()
			 */
			std::string Serialize();
			
			/**
			 * Precondition: The object this function is called upon, is an empty DocumentVector
			 * @see ThywinPacketContent.Deserialize()
			 */
			void Deserialize(const std::string& serializedDocumentVector);
	};

} /* namespace thywin */

#endif /* DOCUMENTVECTOR_H_ */
