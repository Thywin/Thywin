/*
 * DocumentVector.cpp
 *
 *  Created on: 8 mei 2014
 *      Author: Erwin Janssen
 */

#include "DocumentVector.h"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace thywin
{
	
	DocumentVector::DocumentVector()
	{
	}
	DocumentVector::DocumentVector(std::string text)
	{
		if (text.size() == 0)
		{
			return;
		}
		std::transform(text.begin(), text.end(), text.begin(), ::tolower);
		
		bool extractingWord = true;
		if (text.at(0) < 'a' || text.at(0) > 'z')
		{
			extractingWord = false;
		}
		
		unsigned int lastIndex = 0;
		for (unsigned int i = 0; i < text.size(); i++)
		{
			if (extractingWord && (text.at(i) < 'a' || text.at(i) > 'z'))
			{
				(*this)[text.substr(lastIndex, i - lastIndex)]++;
				lastIndex = i;
				extractingWord = false;
			}
			else if (!extractingWord && !(text.at(i) < 'a' || text.at(i) > 'z'))
			{
				lastIndex = i;
				extractingWord = true;
			}
			else if ((i == text.size() - 1) && extractingWord)
			{
				(*this)[text.substr(lastIndex, i - lastIndex + 1)]++;
			}
			// else omitted because there are no other cases that will be handled.
		}
	}
	
	DocumentVector::~DocumentVector()
	{
	}
	
	double DocumentVector::CalculateSimilarity(DocumentVector& documentVector)
	{
		double magnitude = this->GetMagnitude() * documentVector.GetMagnitude();
		if (magnitude == 0)
		{
			return 0;
		}
		else
		{
			return this->DotProduct(documentVector) / magnitude;
		}
	}
	
	double DocumentVector::GetMagnitude()
	{
		double magnitude = 0;
		for (DocumentVector::iterator i = this->begin(); i != this->end(); i++)
		{
			magnitude += std::pow(i->second, 2);
		}
		return std::sqrt(magnitude);
	}
	
	double DocumentVector::DotProduct(DocumentVector& documentVector)
	{
		double dotProduct = 0;
		for (DocumentVector::iterator i = documentVector.begin(); i != documentVector.end(); i++)
		{
			DocumentVector::iterator searchResult = find(i->first);
			if (searchResult != end())
			{
				dotProduct += (i->second) * (searchResult->second);
			}
		}
		return dotProduct;
	}
	
	std::string DocumentVector::Serialize()
	{
		std::stringstream serializedStream;
		for(DocumentVector::iterator i = begin(); i != end(); i++)
		{
			serializedStream << i->first << TP_CONTENT_SEPERATOR << i-> second << TP_CONTENT_SEPERATOR;
		}
		return serializedStream.str();
	}
	
	void DocumentVector::Deserialize(const std::string& serializedDocumentVector)
	{
		std::stringstream serializedStream;
		serializedStream << serializedDocumentVector;
		std::string wordBuffer;
		std::string countBuffer;
		while(std::getline(serializedStream, wordBuffer, TP_CONTENT_SEPERATOR) && std::getline(serializedStream, countBuffer, TP_CONTENT_SEPERATOR))
		{
			(*this)[wordBuffer] = stoi(countBuffer);
		}
	}

} /* namespace thywin */
