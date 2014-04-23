#include "happyhttp.h"
#include <cstdio>
#include <cstring>
#include <stdlib.h>

int count = 0;

void OnBegin(const happyhttp::Response* r, void* userdata)
{
	printf("BEGIN (%d %s)\n", r->getstatus(), r->getreason());
	count = 0;
}

void OnData(const happyhttp::Response* r, void* userdata,
		const unsigned char* data, int n)
{
	// fwrite(data, 1, n, stdout);
	count += n;
}

void OnComplete(const happyhttp::Response* r, void* userdata)
{
	printf("COMPLETE (%d bytes)\n", count);
}

void testRequest(char* site, char* site2)
{
	happyhttp::Connection conn(site, 80);
	conn.setcallbacks(OnBegin, OnData, OnComplete, 0);

	conn.request("GET", site2, 0, 0, 0);

	while (conn.outstanding())
	{
		conn.pump();
	}
}

int main(int argc, char** argv)
{

	try
	{
		testRequest(argv[1], argv[2]);
	}

	catch (happyhttp::Wobbly& e)
	{
		fprintf(stderr, "Exception:\n%s\n", e.what());
	}

	return EXIT_SUCCESS;
}
