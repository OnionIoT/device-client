#ifndef _DEVICE_CLIENT_TYPES_H_
#define _DEVICE_CLIENT_TYPES_H_



// Macro definitions
#define STRING_LENGTH 				256
#define BUFFER_LENGTH				1024

#define DEVICE_SERVER 				"ds.onion.io"
#define DEBUG_SERVER 				"http://zh.onion.io:8081"

#define LISTEN_PATH_TEMPLATE 		"/%s/listen?key=%s"
//#define REPLY_PATH_TEMPLATE 		"/%s/reply?key=%s"
#define REPLY_PATH_TEMPLATE 		"/%s/reply/%s?key=%s"
#define REPLY_POST_TEMPLATE 		"key=%s"

#define JSON_REQUEST_GROUP_KEY		"group"
#define JSON_REQUEST_METHOD_KEY		"method"
#define JSON_REQUEST_PARAM_KEY		"param"
#define JSON_REQUEST_EVENT_ID_KEY	"eventId"

// Data structures
struct deviceClientInfo {
	char 	host[STRING_LENGTH];

	char	devId[STRING_LENGTH];
	char	key[STRING_LENGTH]; 
};


#endif // _DEVICE_CLIENT_TYPES_H_ 
