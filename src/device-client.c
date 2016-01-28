#include <device-client.h>

int curlTest() {
	int 		status	= EXIT_SUCCESS;
	CURL 		*curl;
	CURLcode 	res;

	// initialize curl
	curl_global_init(CURL_GLOBAL_ALL);

	// get a curl handle
	curl 	= curl_easy_init();

	if (!curl){
		return EXIT_FAILURE;
	}

	// set the URL
	curl_easy_setopt(curl, CURLOPT_URL, "http://postit.example.com/moo.cgi");
	// set the POST data
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");

	// perform the request and get return code
	res = curl_easy_perform(curl);
	// check for errors
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res) );
		status 	= EXIT_FAILURE;
	}

	// cleanup
	curl_easy_cleanup(curl);

	return status;
}

int main(int argc, char** argv)
{
	int 	status;

	status 	= curlTest();

	return 0;
}