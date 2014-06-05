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
#include "Logger.h"

namespace thywin
{

	class Parser
	{
		public:
			/*
			 * Creates a Parser which can be used to parse documents.
			 * @param masterIP ip of the master pi
			 * @param masterPort port of the master pi
			 */
			Parser(const std::string& masterIP, const int masterPort);
			
			/**
			 * Default destructor
			 */
			virtual ~Parser();

			/*
			 * Start the Parser, it will run until the Stop method is called
			 * @see Stop()
			 */
			void Run();

			/*
			 * Stop the Parser. Only works if the parser is running
			 * @see Run()
			 */
			void Stop();
		private:
			Logger logger;
			ParserCommunicator communicator;
			bool running;
	};

} /* namespace thywin */

#endif /* PARSER_H_ */
