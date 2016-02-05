#include <ubus-intf.h>

// globals
struct 	blob_buf 	gMsg;

// function prototypes
int 	ubusErrorResponse	(int ubusStatus, char *respUrl);
void 	ubusDataCallback	(struct ubus_request *req, int type, struct blob_attr *msg);


// initialize the blob_buf
void ubusInit()
{
	onionPrint(ONION_SEVERITY_DEBUG_EXTRA, ">>> Running blob_buf_init\n");
	blob_buf_init(&gMsg, 0);
}

// free the blob_buf
void ubusFree()
{
	onionPrint(ONION_SEVERITY_DEBUG_EXTRA, ">>> Running blob_buf_free\n");
	blob_buf_free(&gMsg);
}

// call ubus function 
int ubusCall (char* group, char* method, char* params, char* respUrl)
{
	int 	status;
	int 	groupId;
	char 	*ubus_socket;
	struct 	ubus_context 		*ctx;

	// initialize ubus context
	ctx 	= ubus_connect(ubus_socket);

	if (!ctx) {
		return EXIT_FAILURE;
	}

	// lookup the ubus group
	onionPrint(ONION_SEVERITY_DEBUG_EXTRA, ">>> Running ubus_lookup_id\n");
	status 	= ubus_lookup_id(ctx, group, &groupId);

	if (status == EXIT_SUCCESS) {
		// add the params 
		if (!blobmsg_add_json_from_string(&gMsg, params) ) {
			
			status	= ubusErrorResponse(UBUS_STATUS_INVALID_ARGUMENT, respUrl);
		}

		// make the ubus call
		if (status == EXIT_SUCCESS) {
			onionPrint(ONION_SEVERITY_DEBUG, ">> Launching ubus call\n");
			status 	= ubus_invoke(	ctx, groupId, method, 					// ubus context, group id, method string
									gMsg.head, ubusDataCallback, respUrl,	// blob attr, handler function, priv
									30000);		// timeout

			// check the status
			ubusErrorResponse(status, respUrl);
		}
	}
	else {
		// respond to server saying error finding group
		status 	= ubusErrorResponse(UBUS_STATUS_INVALID_COMMAND, respUrl);
	}

	// clean-up
	ubus_free (ctx);

	return 	status;
}

// respond to server with error message
int ubusErrorResponse(int ubusStatus, char *respUrl)
{
	int 	status 	= EXIT_FAILURE;

	switch (ubusStatus) {
		case UBUS_STATUS_OK:
			status 	= EXIT_SUCCESS;
			break;

		case UBUS_STATUS_INVALID_COMMAND:
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: Requesting invalid ubus group\n");
			curlPost(respUrl, "{\"error\":\"invalid ubus group\"}");
			break;

		case UBUS_STATUS_INVALID_ARGUMENT:
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: Failed to parse ubus parameter data\n");
			curlPost(respUrl, "{\"error\":\"invalid ubus parameter data\"}");
			break;

		case UBUS_STATUS_METHOD_NOT_FOUND:
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: Requesting invalid ubus method\n");
			curlPost(respUrl, "{\"error\":\"invalid ubus method\"}");
			break;

		case UBUS_STATUS_NOT_FOUND:
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: ubus not found\n");
			curlPost(respUrl, "{\"error\":\" ubus not found\"}");
			break;

		case UBUS_STATUS_NO_DATA:
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: ubus: no data\n");
			curlPost(respUrl, "{\"error\":\" ubus: no data\"}");
			break;

		case UBUS_STATUS_PERMISSION_DENIED:
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: ubus permission denied\n");
			curlPost(respUrl, "{\"error\":\" ubus permission denied\"}");
			break;

		case UBUS_STATUS_TIMEOUT:
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: ubus timeout\n");
			curlPost(respUrl, "{\"error\":\" ubus timeout\"}");
			break;

		case UBUS_STATUS_NOT_SUPPORTED:
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: ubus not supported\n");
			curlPost(respUrl, "{\"error\":\" ubus not supported\"}");
			break;

		case UBUS_STATUS_CONNECTION_FAILED:
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: ubus connection failed\n");
			curlPost(respUrl, "{\"error\":\" ubus connection failed\"}");
			break;

		case UBUS_STATUS_UNKNOWN_ERROR:
		default:
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: ubus unknown error\n");
			curlPost(respUrl, "{\"error\":\" ubus unknown error\"}");
			break;
	}

	return 	status;
}

// ubus handler
void ubusDataCallback(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int 	status;
	char 	*body;
	char 	respUrl[BUFFER_LENGTH];

	// check for valid response
	if (!msg) {
		return;
	}

	// read the response data
	strcpy(respUrl, (char *)req->priv);

	// convert the blobmsg json to a string
	body = blobmsg_format_json(msg, true);

	// send the curl response
	status 	= curlPost(respUrl, body);
	
	// clean-up
	free(body);
}