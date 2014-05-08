/*
 * DocumentVector.cpp
 *
 *  Created on: 8 mei 2014
 *      Author: Erwin
 */

#include "DocumentVector.h"
#include <cmath>

namespace thywin {

DocumentVector::DocumentVector() {
	// TODO Auto-generated constructor stub

}

DocumentVector::~DocumentVector() {
	// TODO Auto-generated destructor stub
}

double DocumentVector::CompareTo(DocumentVector* documentVector)
{
	double dotProduct = 0;
	if(this->size() > documentVector->size())
	{
		for(std::map<std::string, unsigned int>::iterator i = this->begin(); i != this->end(); i++)
		{
			std::map<std::string, unsigned int>::iterator searchResult = documentVector->find(i->first);
			if(searchResult != documentVector->end())
			{
				dotProduct += i->second * searchResult->second;
			}
		}
	}
	else
	{
		for(std::map<std::string, unsigned int>::iterator i = documentVector->begin(); i != documentVector->end(); i++)
		{
			std::map<std::string, unsigned int>::iterator searchResult = this->find(i->first);
			if(searchResult != this->end())
			{
				dotProduct += i->second * searchResult->second;
			}
		}
	}

	double magnitudeFirstDocumentVector = 0;
	for(std::map<std::string, unsigned int>::iterator i = this->begin(); i != this->end(); i++)
	{
		magnitudeFirstDocumentVector += std::pow(i->second, 2);
	}
	magnitudeFirstDocumentVector = std::sqrt(magnitudeFirstDocumentVector);

	double magnitudeSecondDocumentVector = 0;
	for(std::map<std::string, unsigned int>::iterator i = documentVector->begin(); i != documentVector->end(); i++)
	{
		magnitudeSecondDocumentVector += std::pow(i->second, 2);
	}
	magnitudeSecondDocumentVector = std::sqrt(magnitudeSecondDocumentVector);



	return dotProduct / (magnitudeFirstDocumentVector * magnitudeSecondDocumentVector);
}

} /* namespace thywin */
