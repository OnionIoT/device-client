#ifndef _DEVICE_CLIENT_H_
#define _DEVICE_CLIENT_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <pthread.h>

#include <onion-debug.h>

#include <device-client-types.h>
#include <device-client-utils.h>

#include "json-c/json.h"


// Function declarations
int 	dcRun 							(char* devId, char* key, char* host);

int 	dcProcessRecvCommand 			(json_object *jobj);

int 	dcProcessUbusCommand 			(json_object *jobj);
int 	dcSendResponse 					(json_object *jobj);


void 	*dcResponseThread				(void *arg);


#endif // _DEVICE_CLIENT_H_ 
