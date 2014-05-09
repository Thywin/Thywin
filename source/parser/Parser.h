/*
 * Parser.h
 *
 *  Created on: May 8, 2014
 *      Author: damonk
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include "ParserCommunicator.h"

namespace thywin
{

class Parser
{
public:
	Parser(std::string documentQueue, std::string URLQueue,
			std::string indexStore);
	virtual ~Parser();

	void Run();

private:
	ParserCommunicator communicator;
	bool running;
};

} /* namespace thywin */

#endif /* PARSER_H_ */
