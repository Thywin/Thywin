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

namespace thywin
{
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


	class URLQueue
	{

	};


	class DocumentQueu
	{

	};


	class Indices
	{

	};


	class Storage
	{
	public:
		URL getURLFromQueue();
	};
}


#endif /* MASTER_HPP_ */
