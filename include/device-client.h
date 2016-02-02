#ifndef _DEVICE_CLIENT_H_
#define _DEVICE_CLIENT_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <onion-debug.h>

#include "libubus.h"

#include "curl-communication.h"


// Macro definition
#define DEVICE_SERVER "ds.onion.io"
#define LISTEN_PATH_TEMPLATE "/%s/listen?key=%s"
#define REPLY_PATH_TEMPLATE "/%s/reply?key=%s"

// Data structures
struct ReplyObj {
  
};

// Function declarations
//int sendReply (ReplyObj reply);


#endif // _DEVICE_CLIENT_H_ 
