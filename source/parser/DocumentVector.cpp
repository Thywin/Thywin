/*
 * DocumentVector.cpp
 *
 *  Created on: 8 mei 2014
 *      Author: Erwin
 */

#include "DocumentVector.h"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace thywin {

DocumentVector::DocumentVector()
{
}
DocumentVector::DocumentVector(std::string text)
{
	if(text.size() == 0)
	{
		return;
	}
	std::transform(text.begin(), text.end(), text.begin(), ::tolower);

	bool extractingWord = true;
	if(text.at(0) < 'a' || text.at(0) > 'z')
	{
		extractingWord = false;
	}
	while(!text.empty())
	{
		for(unsigned int i = 0; i < text.size(); i++)
		{
			if(extractingWord && (text.at(i) < 'a' || text.at(i) > 'z'))
			{
				(*this)[text.substr(0, i)]++;
				text.erase(0, i);
				extractingWord = false;
				break;
			}
			else if(!extractingWord && !(text.at(i) < 'a' || text.at(i) > 'z'))
			{
				text.erase(0, i);
				extractingWord = true;
				break;
			}

			if(i == text.size() - 1)
			{
				if(extractingWord)
				{
					(*this)[text.substr(0, i + 1)]++;
				}
				text.clear();
				break;
			}
		}
	}
}

DocumentVector::~DocumentVector()
{
}

double DocumentVector::CalculateSimilarity(DocumentVector* documentVector)
{
	return this->DotProduct(documentVector) / (this->GetMagnitude() * documentVector->GetMagnitude());
}

double DocumentVector::GetMagnitude()
{
	double magnitude = 0;
	for(std::map<std::string, unsigned int>::iterator i = this->begin(); i != this->end(); i++)
	{
		magnitude += std::pow(i->second, 2);
	}
	return std::sqrt(magnitude);
}

double DocumentVector::DotProduct(DocumentVector* documentVector)
{
	double dotProduct = 0;
	DocumentVector* largestDocumentVector = NULL;
	DocumentVector* smallestDocumentVector = NULL;
	if(this->size() > documentVector->size())
	{
		largestDocumentVector = this;
		smallestDocumentVector = documentVector;
	}
	else
	{
		largestDocumentVector = documentVector;
		smallestDocumentVector = this;
	}

	for(std::map<std::string, unsigned int>::iterator i = smallestDocumentVector->begin(); i != smallestDocumentVector->end(); i++)
	{
		std::map<std::string, unsigned int>::iterator searchResult = largestDocumentVector->find(i->first);
		if(searchResult != largestDocumentVector->end())
		{
			dotProduct += i->second * searchResult->second;
		}
	}
	return dotProduct;
}


} /* namespace thywin */
