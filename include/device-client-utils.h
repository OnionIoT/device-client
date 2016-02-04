#ifndef _DEVICE_CLIENT_UTILS_H_
#define _DEVICE_CLIENT_UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <onion-debug.h>

#include <device-client-types.h>

#include "json-c/json.h"


void 			jsonPrint			(int severity, json_object *jobj, char* tab);
int 			jsonGetString 		(json_object *jobj, char* value);


#endif // _DEVICE_CLIENT_UTILS_H_ 

