#include "happyhttp.h"
#include <cstdio>
#include <cstring>
#include <stdlib.h>

int count = 0;

<<<<<<< HEAD
void
OnBegin (const happyhttp::Response* r, void* userdata)
{
  printf ("BEGIN (%d %s)\n", r->getstatus (), r->getreason ());
  count = 0;
}

void
OnData (const happyhttp::Response* r, void* userdata, const unsigned char* data,
	int n)
{
  // fwrite(data, 1, n, stdout);
  count += n;
}

void
OnComplete (const happyhttp::Response* r, void* userdata)
{
  printf ("COMPLETE (%d bytes)\n", count);
}

void
testRequest (char* site, char* site2)
{
  happyhttp::Connection conn (site, 80);
  conn.setcallbacks (OnBegin, OnData, OnComplete, 0);
=======
void OnBegin(const happyhttp::Response* r, void* userdata) {
	printf("BEGIN (%d %s)\n", r->getstatus(), r->getreason());
	count = 0;
}

void OnData(const happyhttp::Response* r, void* userdata,
		const unsigned char* data, int n) {
	// fwrite(data, 1, n, stdout);
	count += n;
}

void OnComplete(const happyhttp::Response* r, void* userdata) {
	printf("COMPLETE (%d bytes)\n", count);
}

void testRequest(char* site, char* site2) {
	happyhttp::Connection conn(site, 80);
	conn.setcallbacks(OnBegin, OnData, OnComplete, 0);
>>>>>>> d6a32f40c34f5bf44581013cfd4956f47976cf23

  conn.request ("GET", site2, 0, 0, 0);

<<<<<<< HEAD
  while (conn.outstanding ())
    {
      conn.pump ();
    }
=======
	while (conn.outstanding()) {
		conn.pump();
	}
>>>>>>> d6a32f40c34f5bf44581013cfd4956f47976cf23
}

int
main (int argc, char** argv)
{

<<<<<<< HEAD
  try
    {
      testRequest (argv[1], argv[2]);
    }

  catch (happyhttp::Wobbly& e)
    {
      fprintf (stderr, "Exception:\n%s\n", e.what ());
    }
=======
	try {
		testRequest(argv[1], argv[2]);
	}

	catch (happyhttp::Wobbly& e) {
		fprintf(stderr, "Exception:\n%s\n", e.what());
	}
>>>>>>> d6a32f40c34f5bf44581013cfd4956f47976cf23

  return EXIT_SUCCESS;
}
