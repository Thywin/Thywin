/*
 * main.cpp
 *
 *  Created on: 23 apr. 2014
 *      Author: Thomas
 */

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <mutex>
#include <fstream>
#include "master.hpp"
#include "communicator.hpp"

namespace thywin
{

	std::vector<URIElement> Master::URIQueue;
	std::vector<documentElement*> Master::documentQueue;

	void Master::AddURIElementToQueue(struct URIElement element)
	{
		URIQueueMutex.lock();
		Master::URIQueue.insert(Master::URIQueue.end(), element);
		URIQueueMutex.unlock();
	}

	URIElement Master::GetNextURIElementFromQueue()
	{
		URIQueueMutex.lock();
		struct URIElement element;
		if (Master::URIQueue.size() == 0)
		{
			fillURLQueue(); // Temporary queue filling for debug purposes
		}

		if (Master::URIQueue.size() > 0)
		{
			element = Master::URIQueue.at(0);
			Master::URIQueue.erase(URIQueue.begin());
		}
		else
		{
			element.URI = "www.google.nl\0";
			element.hostDocumentRelevance = 0;
		}

		URIQueueMutex.unlock();
		return element;
	}

	void Master::AddDocumentElementToQueue(struct documentElement* element)
	{
		DocumentQueueMutex.lock();
		element->URI.c_str();
		Master::documentQueue.insert(Master::documentQueue.end(), element);
		// unset semaphore is empty
		DocumentQueueMutex.unlock();
	}

	documentElement Master::GetNextDocumentElementFromQueue()
	{
		while (Master::documentQueue.size() < 1)
		{
		}
		// check semaphore is empty
		DocumentQueueMutex.lock();
		documentElement element;
		documentElement* returnElement;
		if (Master::documentQueue.size() > 0)
		{
			returnElement = Master::documentQueue.at(0);
			element.content = returnElement->content;
			element.URI = returnElement->URI;
			Master::documentQueue.erase(Master::documentQueue.begin());
		}
		else
		{
			element.content = std::string();
			element.URI = std::string();
		}
		if (documentQueue.size() < 1)
		{
			// set semaphore is empty
		}

		DocumentQueueMutex.unlock();
		return element;
	}

	void Master::fillURLQueue()
	{
		struct URIElement URIElement;
		URIElement.URI = "www.google.nl\0";
		URIElement.hostDocumentRelevance = 0;
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "www.nu.nl\0";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "stackoverflow.com\0";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://www.thegeekstuff.com\0";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://www.jouwkabel.nl/nl/\0";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "www.facebook.com\0";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "www.han.nl\0";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "www.slamfm.nl\0";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://gd.tuwien.ac.at/languages/c/programming-bbrown/c_075.htm";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://www.senorenrico.nl/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "www.blackbirddevelopment.nl";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://thywin.acunote.com/projects/45558/tasks";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://thywin.com/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI =
				"http://premium.wpmudev.org/blog/wordpress-security-essentials-say-goodbye-to-hackers/?utm_content=buffer9dbf9&utm_medium=social&utm_source=twitter.com&utm_campaign=buffer";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://pippinsplugins.com/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "https://dribbble.com/shots/1452842-iPhone-and-Android-PSD-FREE";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI =
				"http://niklausgerber.com/blog/flat-ui-color-autumn-edition/?utm_content=buffer1d18f&utm_medium=social&utm_source=facebook.com&utm_campaign=buffer";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "pltts.me/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://line25.com/tutorials/create-a-cool-website-with-fancy-scrolling-effects";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://www.fromupnorth.com/web-design-inspiration-930/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI =
				"http://www.bluelimemedia.com/2012/03/01/build-a-custom-image-slider-using-a-custom-post-type/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://buildinternet.com/2009/05/make-an-animated-alphabet-using-keypress-events-in-jquery/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://masonry.desandro.com/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "https://github.com/Thywin/Thywin";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "https://www.youtube.com/watch?v=_sV0S8qWSy0";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "https://mail.google.com/mail/u/0/#inbox";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://en.cppreference.com/w/c";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://www.example-code.com/vcpp/spider.asp";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://www.tutorialspoint.com/postgresql/postgresql_c_cpp.htm";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://www.benjaminarai.com/benjamin_arai/index.php?display=/eclipsecygwingcc.php";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://retis.sssup.it/~scordino/code/logger.html";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI =
				"http://c-mobberley.com/wordpress/2013/10/18/raspberry-pi-installation-of-postgresql-its-simple/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://www.cyberciti.biz/faq/linux-installing-postgresql-database-server/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://www.postgresql.org/docs/8.2/static/tutorial-table.html";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "https://online.han.nl/Pages/default.aspx";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://rooster.han.nl/SchoolplanFT_ICA/?";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "https://webmail.han.nl/rc/?_task=mail";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI =
				"http://www.microsoftstore.com/store?Action=pdp&Locale=nl_NL&SiteID=mseea&WT=srch.1&WT=mc_id.pointitsem_Microsoft+NL_google_Office+15&WT=term.microsoft+office+student&WT=campaign.Office+15&WT=content.yjAG0GtL&WT=source.google&WT=medium.cpc&cid=5371&pcrid=32675620270&pkw=microsoft+office+student&pmt=e&productID=263156100&tid=syjAG0GtL_dc";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://kickass.to/";
		URIQueue.insert(URIQueue.end(), URIElement);
		URIElement.URI = "http://kickass.to/game-of-thrones-s04e05-hdtv-x264-killers-ettv-t9071446.html";

	}

}
