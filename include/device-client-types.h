#ifndef _DEVICE_CLIENT_TYPES_H_
#define _DEVICE_CLIENT_TYPES_H_



// Macro definitions
#define STRING_LENGTH 			256
#define BUFFER_LENGTH			1024

#define DEVICE_SERVER 			"ds.onion.io"
#define DEBUG_SERVER 			"http://zh.onion.io:8081"

#define LISTEN_PATH_TEMPLATE 	"/%s/listen?key=%s"
#define REPLY_PATH_TEMPLATE 	"/%s/reply?key=%s"

// Data structures
struct ReplyObj {
  
};

// Function declarations
//int sendReply (ReplyObj reply);


#endif // _DEVICE_CLIENT_TYPES_H_ 
