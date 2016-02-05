#include <device-client.h>

// global variable
struct 	deviceClientInfo	dcInfo;

// function prototypes
void 	*dcIdentityThread		(void *arg);
void 	*dcResponseThread		(void *arg);


// find deviceId and key
int dcGetIdentity (char* devId, char* key)
{
	int 		status = EXIT_FAILURE;
	pthread_t 	pth;
	void*		info;

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

// main function to launch the listening service
int dcRun (char* devId, char* key, char* host)
{
	int 	status;
	char 	listenPath[STRING_LENGTH];
	char 	request[STRING_LENGTH];

	// initialize the ubus blob msg
	//ubusInit();

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

	onionPrint(ONION_SEVERITY_INFO, "> Connection lost!\n");


	return 	status;
}

// process all commands received from the device server
int dcProcessRecvCommand (json_object *jobj)
{
	int 			status 	= EXIT_FAILURE;
	char			cmd[STRING_LENGTH];
	json_object 	*jCmd;

	pthread_t 		pth;


	jsonPrint(ONION_SEVERITY_DEBUG, jobj, "");

	// check the received json for the 'group' and 'method' objects
	status 	= 	json_object_object_get_ex(jobj, JSON_REQUEST_COMMAND_KEY, &jCmd);

	// check if the 'cmd' object has been found
	if (status != 0) {
		// parse the command string
		status	= 	jsonGetString(jCmd, &cmd );

		if (status == EXIT_FAILURE) {
			return status;
		}

		// read the command
		if (strncmp(cmd, DEVICE_COMMAND_UBUS, strlen(DEVICE_COMMAND_UBUS)) == 0 ) {
			//// UBUS COMMAND
			// create thread to run ubus command and send post response
			pthread_create(&pth, NULL, dcResponseThread, jobj); 
		}
		else if (strncmp(cmd, DEVICE_COMMAND_INIT, strlen(DEVICE_COMMAND_INIT)) == 0 ) {
			//// INIT CONNECTION
			onionPrint(ONION_SEVERITY_INFO, "    > Connection initialized\n");
		}
		else if (strncmp(cmd, DEVICE_COMMAND_HEARTBEAT, strlen(DEVICE_COMMAND_HEARTBEAT)) == 0 ) {
			//// HEARTBEAT
			onionPrint(ONION_SEVERITY_INFO, "    > Connection heartbeat\n");
		}
	}
	else {
		status 	= EXIT_FAILURE;
	}
	

	return 	status;
}


// populate response structure
int dcGenerateResponseUrl (json_object *jobj, char* respUrl)
{
	int 	status;
	char	eventId[STRING_LENGTH];
	char	url[STRING_LENGTH];
	char	postPath[STRING_LENGTH];

	json_object 	*jret;

	onionPrint(ONION_SEVERITY_INFO, "> Sending response to device-server\n"); 

	// parse the event id
	status 	= 	json_object_object_get_ex(jobj, JSON_REQUEST_EVENT_ID_KEY, &jret);
	if (status != 0) {
		// read the strings from the objects
		status	= 	jsonGetString(jret, &eventId );
	}
	else {
		return EXIT_FAILURE;
	}

	// generate the URL to receive the post:
	//	ds.onion.io/<deviceId>/reply/<eventId>
	sprintf(postPath, REPLY_PATH_TEMPLATE, dcInfo.devId, eventId, dcInfo.key);
	sprintf(respUrl, "%s%s", dcInfo.host, postPath);

	return EXIT_SUCCESS;
}

// process a requested ubus command
int dcProcessUbusCommand (json_object *jobj, char* respUrl)
{
	int 			status;
	json_object 	*jGroup;
	json_object 	*jMethod;
	json_object 	*jParam;
	char			group[STRING_LENGTH];
	char			method[STRING_LENGTH];
	const char		*param;

	// check the received json for the 'group' and 'method' objects
	status 	= 	json_object_object_get_ex(jobj, JSON_REQUEST_GROUP_KEY,  &jGroup);
	status 	|=	json_object_object_get_ex(jobj, JSON_REQUEST_METHOD_KEY, &jMethod);
	status 	|=	json_object_object_get_ex(jobj, JSON_REQUEST_PARAM_KEY,  &jParam);

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
	info 	= malloc(sizeof *info);

	// find the deviceId
	sprintf(option, "%s.%s", UCI_ONION_IDENTITY_ROOT, UCI_ONION_IDENTITY_DEVICE_ID_OPTION);
	status 	= uciGet(&option, &value);

	if (status == EXIT_SUCCESS) {
		strcpy(info->devId, value);
	}

	// find the key
	sprintf(option, "%s.%s", UCI_ONION_IDENTITY_ROOT, UCI_ONION_IDENTITY_KEY_OPTION);
	status 	= uciGet(&option, &value);
	if (status == EXIT_SUCCESS) {
		strcpy(info->key, value);
	}

	return 	(void*) info;
}

// threading function to carry out ubus command and send response
void *dcResponseThread(void *arg)
{
	int 			status;
	json_object 	*jobj;
	char 			respUrl[BUFFER_LENGTH];

	onionPrint(ONION_SEVERITY_DEBUG, "\n>> RESPONSE THREAD!\n");

	// convert the argument to json object
	jobj 	= (json_object*)arg;

	// setup the response to the server
	status 	= dcGenerateResponseUrl(jobj, &respUrl);

	// carry out the ubus command
	if (status == EXIT_SUCCESS) {
		status 	= dcProcessUbusCommand(jobj, &respUrl);
	}

	return NULL;
}

