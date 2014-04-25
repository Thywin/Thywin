#include "happyhttp.h"
#include <cstdio>
#include <cstring>
#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>

int count = 0;
int fileID = -1;
void OnBegin(const happyhttp::Response* r, void* userdata)
{
	printf("BEGIN (%d %s)\n", r->getstatus(), r->getreason());
	count = 0;
}

void OnData(const happyhttp::Response* r, void* userdata,
		const unsigned char* data, int n)
{
	// fwrite(data, 1, n, stdout);

	if (write(fileID, data,n) < 1) {
		perror("failed to write");
	} else {
		//printf("%c",data);
		fwrite(data, 1, n, stdout);
	}
	count += n;
}

void OnComplete(const happyhttp::Response* r, void* userdata)
{
	printf("COMPLETE (%d bytes)\n\n", count);
}

void sendRequestTest(char* site, char* site2)
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
	char *collection[3] = {"www.google.nl","kooisoftware.nl","www.nu.nl"};
	try
	{
		for (int i = 0; i < 3; i++) {
			fileID = open(collection[i],O_CREAT | O_WRONLY, 0777);
			if (fileID < 0) {
				perror("failed to create file");
			} else {
				printf("Opening: %s",collection[i]);
				sendRequestTest(collection[i],"/");
				printf("Closing file\n");
				close(fileID);
			}
		}
		//sendRequestTest(argv[1], argv[2]);
	}

	catch (happyhttp::Wobbly& e)
	{
		fprintf(stderr, "Exception:\n%s\n", e.what());
	}

	return EXIT_SUCCESS;
}
