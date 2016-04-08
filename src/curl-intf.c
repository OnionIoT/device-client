#include <curl-intf.h>
#include "curl/curl.h"

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
	curl_global_init(CURL_GLOBAL_ALL);
}

// global curl cleanup
int curlCleanup() 
{
	curl_global_cleanup();
}

size_t onListenResponse (void *buffer, size_t size, size_t nmemb, void *userp){
	printf("ZH: \r\n");
	printf(buffer);
}

// parse data received from the server
size_t recvDataCallbackOrig(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	int 			status;
	json_object 	*jobj;
	size_t 			realSize = size * nmemb;

	// find current system time
	time_t t;
    time(&t);

	onionPrint(ONION_SEVERITY_DEBUG, ">> Write_callback: received '%d' bytes, time: '%s'\n", realSize, ctime(&t) );
	if (realSize > 0) {
		//onionPrint(ONION_SEVERITY_DEBUG, "   >> received data '%s'\n", ptr);

		// attempt to parse the received data as json
		jobj 	= json_tokener_parse(ptr);

		if (jobj != NULL) {
			onionPrint(ONION_SEVERITY_DEBUG, ">> Write_callback: valid json: '%s'\n", ptr);
			// cleanup the json object
			json_object_put(jobj);

			// device client - process the command received from the server
			status 	= dcProcessRecvCommand(ptr);
		}
	}


	return 	realSize;
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
	if (realSize > 0 && ptr[0] == '{' && ptr[realSize-1] == '}') {
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

	onionPrint(ONION_SEVERITY_DEBUG, ">> curlListen\n");
	// init the curl session
	handle 	= curl_easy_init();
	onionPrint(ONION_SEVERITY_DEBUG, ">> curlListen: got handle\n");

	// set the options
	sprintf(getUrl, "%s/%s", host, request);
	curl_easy_setopt(handle, CURLOPT_URL, getUrl);

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
		return EXIT_FAILURE;
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
		// set content type
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");

		// set the URL that will receive the POST
		curl_easy_setopt(curl, CURLOPT_URL, url);
		// set headers
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		// specify the POST data
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

		// Perform the request, res will get the return code
		res = curl_easy_perform(curl);
		
		// Check for errors
		if(res != CURLE_OK) {
			onionPrint(ONION_SEVERITY_FATAL, "Error: %s\n", curl_easy_strerror(res));
			status 	=  EXIT_FAILURE;
		}

		
		// cleanup
		curl_easy_cleanup(curl);
	}

	return status;
}





