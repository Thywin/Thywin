/*
 * RelevanceDeterminator.cpp
 *
 *  Created on: 8 mei 2014
 *      Author: Erwin
 */

#include "RelevanceDeterminator.h"
#include <vector>
#include <iostream>
#include <cmath>

RelevanceDeterminator::RelevanceDeterminator()
{
	// TODO Auto-generated constructor stub
}

RelevanceDeterminator::~RelevanceDeterminator()
{
	// TODO Auto-generated destructor stub
}

double RelevanceDeterminator::DetermineRelevance(std::map<std::string, unsigned int> firstDocumentVector, std::map<std::string, unsigned int> secondDocumentVector)
{

	double dotProduct = 0;
	if(firstDocumentVector.size() > secondDocumentVector.size())
	{
		for(std::map<std::string, unsigned int>::iterator i = firstDocumentVector.begin(); i != firstDocumentVector.end(); i++)
		{
			std::map<std::string, unsigned int>::iterator searchResult = secondDocumentVector.find(i->first);
			if(searchResult != secondDocumentVector.end())
			{
				dotProduct += i->second * searchResult->second;
			}
		}
	}
	else
	{
		for(std::map<std::string, unsigned int>::iterator i = secondDocumentVector.begin(); i != secondDocumentVector.end(); i++)
		{
			std::map<std::string, unsigned int>::iterator searchResult = firstDocumentVector.find(i->first);
			if(searchResult != firstDocumentVector.end())
			{
				dotProduct += i->second * searchResult->second;
			}
		}
	}

	double magnitudeFirstDocumentVector = 0;
	for(std::map<std::string, unsigned int>::iterator i = firstDocumentVector.begin(); i != firstDocumentVector.end(); i++)
	{
		magnitudeFirstDocumentVector += std::pow(i->second, 2);
	}
	magnitudeFirstDocumentVector = std::sqrt(magnitudeFirstDocumentVector);

	double magnitudeSecondDocumentVector = 0;
	for(std::map<std::string, unsigned int>::iterator i = secondDocumentVector.begin(); i != secondDocumentVector.end(); i++)
	{
		magnitudeSecondDocumentVector += std::pow(i->second, 2);
	}
	magnitudeSecondDocumentVector = std::sqrt(magnitudeSecondDocumentVector);



	return dotProduct / (magnitudeFirstDocumentVector * magnitudeSecondDocumentVector);
}

