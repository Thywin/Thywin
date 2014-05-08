/*
 * master.hpp
 *
 *  Created on: 7 mei 2014
 *      Author: Thomas
 */

#ifndef MASTER_HPP_
#define MASTER_HPP_
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>


namespace thywin
{
	const short URL = 1;
	const short DOCUMENT = 2;
	const short GET = 1;
	const short PUT = 2;
	struct requestContainer {
		short type;
		short action;
		int size;
		//std::stringstream content;
		//std::stringstream meta;
	};


	class master
	{
	public:
		int setupServer(int port);
		void handleConnection(void *socket);

	private:


	};


	class RequestHandler
	{
	public:
		RequestHandler(int clientSocket);
	};


	class URL
	{
	public:
		std::string getURL();
		double getRelevance();

	private:
		std::string URL;
		double relevance;
	};


	class Document
	{
	public:
		std::string getContent();

	private:
		std::string content;
	};

	class Index
	{
	private:
		//map<std::string,int> documentVector;
	};

	class Indices
	{

	};


	class Storage
	{
	public:
		void getURLFromQueue();
	};
}


#endif /* MASTER_HPP_ */
