#include <device-client.h>

// global variable
struct 	deviceClientInfo	dcInfo;

// main function to launch the listening service
int dcRun (char* devId, char* key, char* host)
{
	int 	status;
	char 	listenPath[STRING_LENGTH];
	char 	request[STRING_LENGTH];

	// store pertinent info globally
	strcpy(dcInfo.host, 	host);
	strcpy(dcInfo.devId, 	devId);
	strcpy(dcInfo.key, 		key);

	// generate the http request:
	//	"GET /dev1/listen?key=key1 HTTP/1.1\r\nHost: zh.onion.io\r\n\r\n";
	sprintf(listenPath, LISTEN_PATH_TEMPLATE, devId, key);
	sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", listenPath, host);

	// start listening to the device server
	curlListen(host, request);


	return 	status;
}

// process all commands received from the device server
int dcProcessRecvCommand (json_object *jobj)
{
	int 			status 	= EXIT_FAILURE;
	json_object 	*jGroup;
	json_object 	*jMethod;

	pthread_t 		pth;


	jsonPrint(ONION_SEVERITY_DEBUG, jobj, "");

	// check the received json for the 'group' and 'method' objects
	status 	= 	json_object_object_get_ex(jobj, JSON_REQUEST_GROUP_KEY, &jGroup);
	status 	|=	json_object_object_get_ex(jobj, JSON_REQUEST_METHOD_KEY, &jMethod);

	// check if the 'group' and 'method' object are both found
	if (status != 0) {
		// create thread
		pthread_create(&pth, NULL, dcResponseThread, jobj); 
	}
	

	return 	status;
}

// process a requested ubus command
// LAZAR: add json object for ubus return
int dcProcessUbusCommand (json_object *jobj)
{
	int 			status;
	json_object 	*jGroup;
	json_object 	*jMethod;
	char			group[STRING_LENGTH];
	char			method[STRING_LENGTH];

	// check the received json for the 'group' and 'method' objects
	status 	= 	json_object_object_get_ex(jobj, JSON_REQUEST_GROUP_KEY, &jGroup);
	status 	|=	json_object_object_get_ex(jobj, JSON_REQUEST_METHOD_KEY, &jMethod);

	// check if the 'group' and 'method' object are both found
	if (status != 0) {
		onionPrint(ONION_SEVERITY_DEBUG, ">> Found 'group' and 'method' objects\n"); 

		// read the strings from the objects
		status	= 	jsonGetString(jGroup, &group);
		status	|= 	jsonGetString(jMethod, &method);

		// check that strings were read properly
		if (status == EXIT_SUCCESS) {
			onionPrint(ONION_SEVERITY_INFO, ">> Received command request for '%s' group, '%s' function\n", group, method);

			// LAZAR: add ubus call
		}
	}

	return 	status;
}

// send a response to the server
int dcSendResponse (json_object *jobj)
{
	int 	status;
	char	eventId[STRING_LENGTH];
	char	url[STRING_LENGTH];
	char	postPath[STRING_LENGTH];
	char	postData[STRING_LENGTH];

	json_object 	*jret;

	onionPrint(ONION_SEVERITY_INFO, "> Sending response to device-server\n"); 

	// parse the event id
	status 	= 	json_object_object_get_ex(jobj, JSON_REQUEST_EVENT_ID_KEY, &jret);
	if (status != 0) {
		// read the strings from the objects
		status	= 	jsonGetString(jret, &eventId);
	}
	else {
		return EXIT_FAILURE;
	}

	// generate the URL to receive the post:
	//	ds.onion.io/<deviceId>/reply/<eventId>
	sprintf(postPath, REPLY_PATH_TEMPLATE, dcInfo.devId, eventId, dcInfo.key);
	sprintf(url, "%s%s", dcInfo.host, postPath);

	// generate the data to POST
	sprintf(postData, "{\"success\":\"OK\"}");

	// send the POST
	status 	= curlPost(url, postData);


	return 	status;
}

// threading function to carry out ubus command and send response
void *dcResponseThread(void *arg)
{
	int 			status;
	json_object 	*jobj;

	onionPrint(ONION_SEVERITY_DEBUG, "\n>> RESPONSE THREAD!\n");

	// convert the argument to json object
	jobj 	= (json_object*)arg;

	// carry out the ubus command
	status 	= dcProcessUbusCommand(jobj);

	// send the response to the server
	status 	= dcSendResponse(jobj);


	return NULL;
}

