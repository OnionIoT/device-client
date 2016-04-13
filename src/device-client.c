#include <device-client.h>

// global variable
struct 	deviceClientInfo	dcInfo;

// function prototypes
void 	*dcIdentityThread		(void *arg);
void 	*dcResponseThread		(void *arg);
void 	dcRespondError 			(char* url, char *msg);


// find deviceId and key
int dcGetIdentity (char* devId, char* key)
{
	int 		status = EXIT_FAILURE;
	pthread_t 	pth;
	void*		info 	= NULL;

	// launch thread to read device ID and key
	pthread_create(&pth, NULL, dcIdentityThread, NULL);
	pthread_join(pth, &info);

	// check that the read succeeded
	if 	(	info != NULL &&
			strlen((*(struct deviceClientInfo*)info).devId) > 0 &&
			strlen((*(struct deviceClientInfo*)info).key) > 0
		)
	{
		strncpy(devId, 	(*(struct deviceClientInfo*)info).devId, 	strlen( (*(struct deviceClientInfo*)info).devId) );
		strncpy(key, 	(*(struct deviceClientInfo*)info).key,		strlen( (*(struct deviceClientInfo*)info).key) );
		onionPrint(ONION_SEVERITY_INFO, "> Omega identified as device '%s'\n", devId);
		status = EXIT_SUCCESS;
	}

	free(info);
	return 	status;
}

// store the required info in the dcInfo struct
int dcSetup(char* devId, char* key, char* host)
{
	// store pertinent info globally
	strncpy(dcInfo.host, 	host,	strlen(host) );
	strncpy(dcInfo.devId, 	devId, 	strlen(devId) );
	strncpy(dcInfo.key, 	key,	strlen(key) );

	return EXIT_SUCCESS;
}

// main function to launch the listening service
int dcRun (int debugLevel)
{
	int 	status;
	char 	listenPath[STRING_LENGTH];

	onionPrint(ONION_SEVERITY_DEBUG, ">> Starting device client\n");

	// generate the listen path
	//	"GET /dev1/listen?key=key1 HTTP/1.1\r\nHost: zh.onion.io\r\n\r\n";
	sprintf(listenPath, LISTEN_PATH_TEMPLATE, dcInfo.devId, dcInfo.key);

	// start listening to the device server
	onionPrint(ONION_SEVERITY_DEBUG, ">> Starting listening...\n");
	status 	= curlListen(dcInfo.host, listenPath, debugLevel);

	onionPrint(ONION_SEVERITY_INFO, "> Connection lost!\n");


	return 	status;
}

// process all commands received from the device server
int dcProcessRecvCommand (char* receivedData)
{
	int 			status 	= EXIT_FAILURE;
	char			cmd[STRING_LENGTH];
	json_object 	*jObj;

	pthread_t 		pth;

	// parse the string-form json
	jObj 	= json_tokener_parse(receivedData);

	if (jObj != NULL) {
		jsonPrint(ONION_SEVERITY_DEBUG, jObj, "");

		// create thread to process the command and send post response
		pthread_create(&pth, NULL, dcResponseThread, jObj);
		pthread_join(pth, NULL);	// join the thread so the thread memory is freed
	}

	return 	status;
}


// populate response structure
int dcGenerateResponseUrl (json_object *jObj, char* respUrl, char* id)
{
	int 	status;
	char	eventId[STRING_LENGTH];
	char	url[STRING_LENGTH];
	char	postPath[STRING_LENGTH];

	json_object 	*jret;

	onionPrint(ONION_SEVERITY_INFO, "> Sending response to device-server\n"); 

	// parse the event id
	status 	= 	json_object_object_get_ex(jObj, JSON_REQUEST_EVENT_ID_KEY, &jret);
	if (status != 0) {
		// read the strings from the objects
		status	= 	jsonGetString(jret, &eventId );
		// clean-up
		json_object_put(jret);
	}
	else {
		// use the command argument as the id
		//strncpy(eventId, id, strlen(id) );
		strcpy(eventId, id);
	}

	// generate the URL to receive the post:
	//	ds.onion.io/<deviceId>/reply/<eventId>
	sprintf(postPath, REPLY_PATH_TEMPLATE, dcInfo.devId, eventId, dcInfo.key);
	sprintf(respUrl, "%s%s", dcInfo.host, postPath);

	return EXIT_SUCCESS;
}

// process a requested ubus command
int dcProcessUbusCommand (json_object *jObj, char* respUrl)
{
	int 			status;
	json_object 	*jGroup;
	json_object 	*jMethod;
	json_object 	*jParam;
	char			group[STRING_LENGTH];
	char			method[STRING_LENGTH];
	const char		*param;

	// check the received json for the 'group' and 'method' objects
	status 	= 	json_object_object_get_ex(jObj, JSON_REQUEST_GROUP_KEY,  &jGroup);
	status 	|=	json_object_object_get_ex(jObj, JSON_REQUEST_METHOD_KEY, &jMethod);
	status 	|=	json_object_object_get_ex(jObj, JSON_REQUEST_PARAM_KEY,  &jParam);

	// check if the 'group' and 'method' object are both found
	if (status != 0) {
		onionPrint(ONION_SEVERITY_DEBUG, ">> Found 'group' and 'method' objects\n"); 

		// initialize the ubus blob msg
		ubusInit();

		// read the strings from the objects
		status	= 	jsonGetString(jGroup, &group);
		status	|= 	jsonGetString(jMethod, &method);

		// convert param object to string
		param 	= json_object_to_json_string(jParam);	

		// check that strings were read properly
		if (status == EXIT_SUCCESS) {
			onionPrint(ONION_SEVERITY_INFO, "> Received command request for '%s' group, '%s' function\n", group, method);
			onionPrint(ONION_SEVERITY_DEBUG, ">> Parameters: '%s'\n", param);

			// make the ubus call
			status = ubusCall(group, method, param, respUrl);
		}

		// clean-up the ubus blob msg
		ubusFree();
	}

	return 	status;
}

// respond with an error
void dcRespondError (char* url, char *msg)
{
	int 	status;
	char 	resp[BUFFER_LENGTH];

	// generate and send the error response
	sprintf(resp, RESPONSE_ERROR_TEMPLATE, msg);
	status 	= curlPost(url, resp);
}

//// multi-threaded functions
// threading function to carry out uci calls and find deviceId and key
void *dcIdentityThread(void *arg)
{
	int 	status;
	char 	option[STRING_LENGTH];
	char 	value[STRING_LENGTH];
	struct	deviceClientInfo	*info;

	onionPrint(ONION_SEVERITY_DEBUG, "\n>> IDENTITY THREAD!\n");

	// allocate the structure
	status 	= EXIT_FAILURE;
	info 	= malloc(sizeof *info);

	// find the deviceId
	sprintf(option, "%s.%s", UCI_ONION_IDENTITY_ROOT, UCI_ONION_IDENTITY_DEVICE_ID_OPTION);
	status 	= uciGet(&option, &value);

	if (status == EXIT_SUCCESS) {
		strncpy(info->devId, value, strlen(value));
	}

	// find the key
	sprintf(option, "%s.%s", UCI_ONION_IDENTITY_ROOT, UCI_ONION_IDENTITY_KEY_OPTION);
	status 	= uciGet(&option, &value);
	if (status == EXIT_SUCCESS) {
		strncpy(info->key, value, strlen(value));
	}

	return 	(void*) info;
}

// threading function to carry out ubus command and send response
void *dcResponseThread(void *arg)
{
	int 			status;
	json_object 	*jObj;
	json_object 	*jCmd;
	char			cmd[BUFFER_LENGTH];
	char 			respUrl[BUFFER_LENGTH];

	onionPrint(ONION_SEVERITY_DEBUG, "\n>> RESPONSE THREAD!\n");

	// convert the argument to json object
	jObj 	= (json_object*)arg;

	// read the command object
	status 	= 	json_object_object_get_ex(jObj, JSON_REQUEST_COMMAND_KEY, &jCmd);

	if (status != 0) {
		// parse the command string
		status	= 	jsonGetString(jCmd, &cmd);

		if (status == EXIT_FAILURE) {
			dcRespondError (respUrl, "Could not read 'command' object!\n");
			return NULL;
		}

		// setup the response to the server
		status 	= dcGenerateResponseUrl(jObj, &respUrl, cmd);

		// read the command
		if (strncmp(cmd, DEVICE_COMMAND_UBUS, strlen(DEVICE_COMMAND_UBUS)) == 0 ) {
			//// UBUS COMMAND
			// carry out the ubus command
			status 	= dcProcessUbusCommand(jObj, &respUrl);
		}
		else if (strncmp(cmd, DEVICE_COMMAND_INIT, strlen(DEVICE_COMMAND_INIT)) == 0 ) {
			//// INIT CONNECTION
			onionPrint(ONION_SEVERITY_INFO, "    > Connection initialized\n");

			// generate and send the response
			sprintf(cmd, RESPONSE_INIT_TEMPLATE, DEVICE_CLIENT_VERSION, DEVICE_TYPE);
			status 	= curlPost(respUrl, cmd);
		}
		else if (strncmp(cmd, DEVICE_COMMAND_HEARTBEAT, strlen(DEVICE_COMMAND_HEARTBEAT)) == 0 ) {
			//// HEARTBEAT
			onionPrint(ONION_SEVERITY_INFO, "    > Connection heartbeat\n");
			
			// send the response
			status 	= curlPost(respUrl, RESPONSE_HEARTBEAT_TEMPLATE);
		}
		else {
			//// INVALID COMMAND
			dcRespondError (respUrl, "Invalid command!\n");
		}
	}
	else {
		dcRespondError (respUrl, "Could not find 'command' object!\n");
	}

	// free the json object
	json_object_put(jObj);


	return NULL;
}

