#include <curl-intf.h>
#include "curl/curl.h"

// global variables
struct curl_slist 	*gCurlJsonHeaders;


/* Auxiliary function that waits on the socket. */ 
static int wait_on_socket(curl_socket_t sockfd, int for_recv, long timeout_ms)
{
	struct timeval tv;
	fd_set infd, outfd, errfd;
	int res;

	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec= (timeout_ms % 1000) * 1000;

	FD_ZERO(&infd);
	FD_ZERO(&outfd);
	FD_ZERO(&errfd);

	FD_SET(sockfd, &errfd); /* always check for error */ 

	if(for_recv)
	{
		FD_SET(sockfd, &infd);
	}
	else
	{
		FD_SET(sockfd, &outfd);
	}

	/* select() returns the number of signalled sockets or -1 */ 
	res = select(sockfd + 1, &infd, &outfd, &errfd, &tv);
	return res;
}

// global curl init
int curlInit() 
{
	curl_global_init(CURL_GLOBAL_SSL);

	// set content type	to json
	gCurlJsonHeaders = curl_slist_append(gCurlJsonHeaders, "Accept: application/json");
	gCurlJsonHeaders = curl_slist_append(gCurlJsonHeaders, "Content-Type: application/json");
}

// global curl cleanup
int curlCleanup() 
{
	// free the json headers
	curl_slist_free_all(gCurlJsonHeaders);

	curl_global_cleanup();
}

// parse data received from the server
size_t recvDataCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	int 			status;
	size_t 			realSize = size * nmemb;

	// find current system time
	time_t t;
    time(&t);

	onionPrint(ONION_SEVERITY_DEBUG, ">> Write_callback: received '%d' bytes, time: '%s'\n", realSize, ctime(&t) );
	if (isJson(ptr, realSize) == 1) {
		onionPrint(ONION_SEVERITY_DEBUG, ">> Write_callback: valid json: '%s'\n", ptr);

		// device client - process the command received from the server
		status 	= dcProcessRecvCommand(ptr);
	}


	return 	realSize;
}


// listen to device server
//	HTTP GET request w/ device id and secret
// 	Callback to parse data received from the server
int curlListen (char* host, char* request, int debugLevel)
{
	CURL 		*handle;
	CURLcode 	res;
	int 		status	= EXIT_SUCCESS;
	char 		errbuf[CURL_ERROR_SIZE];

	char 		getUrl[STRING_LENGTH];

	onionPrint(ONION_SEVERITY_DEBUG_EXTRA, ">>> curlListen\n");
	// init the curl session
	handle 	= curl_easy_init();
	onionPrint(ONION_SEVERITY_DEBUG_EXTRA, ">>> curlListen: got handle\n");

	// set the options
	sprintf(getUrl, "%s/%s", host, request);
	curl_easy_setopt(handle, CURLOPT_URL, getUrl);
	// set options to disconnect if connection speed is, on average, less than 1 byte/s for 60 seconds
	curl_easy_setopt(handle, CURLOPT_LOW_SPEED_LIMIT, 1L);
	curl_easy_setopt(handle, CURLOPT_LOW_SPEED_TIME, 60L);

	// register write data callback to receive data from the server
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, recvDataCallback);

	// set the debugging options
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errbuf);
	if (debugLevel >= DC_DEBUG_LEVEL_CURL) {
		curl_easy_setopt(handle, CURLOPT_VERBOSE, 	1L);
		//curl_easy_setopt(handle, CURLOPT_HEADER, 	1L);
	}

	// empty out the error buffer
	errbuf[0] = 0;

	// perform the action
	onionPrint(ONION_SEVERITY_DEBUG, ">> Sending request to '%s'\n", getUrl);
	res 	= curl_easy_perform(handle);
	if(CURLE_OK != res)
	{
		onionPrint(ONION_SEVERITY_FATAL, "Error: curl_easy_perform: %s (%d)\n", errbuf, res);
		status = EXIT_FAILURE;
	}

	onionPrint(ONION_SEVERITY_DEBUG, ">> Completed GET\n");
	
	curl_easy_cleanup(handle);
	return status;
}

// perform an http post operation
int curlPost(char* url, char* postData)
{
	int status;
	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL;	// http headers to send with request

	onionPrint(ONION_SEVERITY_DEBUG, ">> Sending POST to url: '%s', post data: '%s'\n", url, postData);

	// get the handle
	curl = curl_easy_init();

	// check that the handle is ok
	if(curl) {
		// future: if POST is not json, set the headers here

		// set the URL that will receive the POST
		curl_easy_setopt(curl, CURLOPT_URL, url);
		// set headers
		// future: change the headers if required
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, gCurlJsonHeaders);
		// specify the POST data
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

		// Perform the request, res will get the return code
		res = curl_easy_perform(curl);
		
		// Check for errors
		onionPrint(ONION_SEVERITY_DEBUG, ">> POST Sent! (url: '%s')\n", url);
		if(res != CURLE_OK) {
			onionPrint(ONION_SEVERITY_FATAL, "Error: %s\n", curl_easy_strerror(res));
			status 	=  EXIT_FAILURE;
		}

		
		// cleanup
		if (headers != NULL) {
			curl_slist_free_all(headers);
		}
		curl_easy_cleanup(curl);
	}

	return status;
}





