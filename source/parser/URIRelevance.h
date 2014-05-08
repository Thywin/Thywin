/*
 * UriRelevance.h
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#ifndef URIRELEVANCE_H_
#define URIRELEVANCE_H_

#include <string>

namespace thywin
{

typedef struct URIRelevance
{
	double score;
	std::string URI;
} URIRelevance;

} /* namespace thywin */

#endif /* URIRELEVANCE_H_ */
