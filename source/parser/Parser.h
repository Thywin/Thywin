/*
 * Parser.h
 *
 *  Created on: May 8, 2014
 *      Author: Imre Woudstra
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
			/*
			 * Creates a Parser which can be used to parse documents.
			 */
			Parser(std::string documentQueue, std::string URLQueue, std::string indexStore);
			virtual ~Parser();

			/*
			 * Run the Parser which will run until the Stop method is called
			 * @see Stop()
			 */
			void Run();

			/*
			 * Stop the Parser only works if the parser is running
			 * @see Run()
			 */
			void Stop();
		private:
			ParserCommunicator communicator;
			bool running;
	};

} /* namespace thywin */

#endif /* PARSER_H_ */
