#ifndef _DEVICE_CLIENT_TYPES_H_
#define _DEVICE_CLIENT_TYPES_H_



// Macro definitions
#define STRING(s) #s

#ifndef DEVICE_CLIENT_VERSION
#define DEVICE_CLIENT_VERSION		"0.0"
#else
//#define DEVICE_CLIENT_VERSION 		"DEVICE_CLIENT_VERSION"
#endif

#ifndef DEVICE_TYPE
#define DEVICE_TYPE					"UNKNOWN"
#else
//#define DEVICE_TYPE 				"DEVICE_TYPE"
#endif

#define STRING_LENGTH 				256
#define BUFFER_LENGTH				1024

#define UCI_ONION_IDENTITY_ROOT				"onion.cloud"
#define UCI_ONION_IDENTITY_DEVICE_ID_OPTION	"deviceId"
#define UCI_ONION_IDENTITY_KEY_OPTION		"secret"

#define DEVICE_SERVER 				"ds.onion.io"
#define DEBUG_SERVER 				"http://zh.onion.io:8081"

#define LISTEN_PATH_TEMPLATE 		"/%s/listen?key=%s"
#define REPLY_PATH_TEMPLATE 		"/%s/reply/%s?key=%s"
#define REPLY_POST_TEMPLATE 		"key=%s"

#define RESPONSE_INIT_TEMPLATE 			"{\"version\":\"%s\", \"device\":\"%s\"}"
#define RESPONSE_HEARTBEAT_TEMPLATE 	"{\"heartbeat\":\"ok\"}"
#define RESPONSE_ERROR_TEMPLATE 		"{\"error\":\"%s\"}"

#define JSON_REQUEST_COMMAND_KEY	"cmd"
#define JSON_REQUEST_GROUP_KEY		"group"
#define JSON_REQUEST_METHOD_KEY		"method"
#define JSON_REQUEST_PARAM_KEY		"param"
#define JSON_REQUEST_EVENT_ID_KEY	"eventId"

#define DEVICE_COMMAND_INIT 		"init"
#define DEVICE_COMMAND_HEARTBEAT	"heartbeat"
#define DEVICE_COMMAND_UBUS 		"ubus"


// Data structures
struct deviceClientInfo {
	char 	host[STRING_LENGTH];

	char	devId[STRING_LENGTH];
	char	key[STRING_LENGTH]; 
};


#endif // _DEVICE_CLIENT_TYPES_H_ 
