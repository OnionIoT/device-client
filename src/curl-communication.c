#include <curl-communication.h>

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

int parseRecvData(int nread, char* buf)
{
	int 	i;
	char 	*ref;
	json_object 	*jobj;

	onionPrint(ONION_SEVERITY_DEBUG, "Data: ");

	// parse the json from the received data into a char
	ref = &buf[0];
	for(i =0; i<nread; i++){
		if(ref[0]!='{'){
			ref++;
		}
		else {
			break;
		}
	}

	onionPrint(ONION_SEVERITY_DEBUG, ref);

	// parse the char into a json object
	if (strlen(ref) > 0) {
		jobj = json_tokener_parse(ref);
		jsonPrint(ONION_SEVERITY_DEBUG, jobj);
	}
	
}

// listen to device server
int dsListen ()
{
	CURL 		*req;
	CURLcode 	res;
	int 		status	= EXIT_SUCCESS;

	/* Minimalistic http request */ 
	const char *request = "GET /dev1/listen?key=key1 HTTP/1.1\r\nHost: ds.onion.io\r\n\r\n";
	curl_socket_t sockfd; /* socket */ 
	long sockextr;
	size_t iolen;
	curl_off_t nread;
	
	// initialize request
	req 	= curl_easy_init();
	if(!req){
		return EXIT_FAILURE;
	}

	// set the URL
	curl_easy_setopt(req, CURLOPT_URL, "http://zh.onion.io:8081");
	// 	curl_easy_setopt(req, CURLOPT_TIMEOUT, 30L);
	curl_easy_setopt(req, CURLOPT_CONNECT_ONLY, 1L);
	res = curl_easy_perform(req);
	if(CURLE_OK != res)
	{
		onionPrint(ONION_SEVERITY_FATAL, "Error: %s\n", strerror(res));
		return EXIT_FAILURE;
	}
	res = curl_easy_getinfo(req, CURLINFO_LASTSOCKET, &sockextr);
 
	if(CURLE_OK != res)
	{
	  onionPrint(ONION_SEVERITY_FATAL, "Error: %s\n", curl_easy_strerror(res));
	  return EXIT_FAILURE;
	}

	sockfd = sockextr;
 
	/* wait for the socket to become ready for sending */ 
	if(!wait_on_socket(sockfd, 0, 60000L))
	{
	  onionPrint(ONION_SEVERITY_FATAL, "Error: timeout.\n");
	  return EXIT_FAILURE;
	}
	
	onionPrint(ONION_SEVERITY_DEBUG, "Sending request.\n");
	/* Send the request. Real applications should check the iolen
	 * to see if all the request has been sent */ 
	res = curl_easy_send(req, request, strlen(request), &iolen);
 
	if(CURLE_OK != res)
	{
	  onionPrint(ONION_SEVERITY_FATAL, "Error: %s\n", curl_easy_strerror(res));
	  return 1;
	}
	
	/* read the response */ 
	onionPrint(ONION_SEVERITY_DEBUG, "Reading response.\n");
	for(;;)
	{
		char buf[1024];
		memset(&buf[0], 0, sizeof(buf));	// clear the buffer

		onionPrint(ONION_SEVERITY_DEBUG, "Waiting on socket...\n");
		wait_on_socket(sockfd, 1, 60000L);
		res = curl_easy_recv(req, buf, 1024, &iolen);

		if(CURLE_OK != res) {
			break;
		}

		nread = (curl_off_t)iolen;

		onionPrint(ONION_SEVERITY_DEBUG, "Received %" CURL_FORMAT_CURL_OFF_T " bytes.\n", nread);
		onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "Received data: '%s'\n", buf);

		// parse the json from the received data;
		status 	= parseRecvData(nread, buf);
	}
	
	curl_easy_cleanup(req);
	return status;
}

int doPost(url, body){
	return 0;
}