#ifndef _UBUS_INTF_H_
#define _UBUS_INTF_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <onion-debug.h>

#include <device-client-types.h>

#include "libubus.h"
#include "libubox/blobmsg_json.h"


void 			ubusInit		();
void 			ubusFree		();

int 			ubusCall 		(char* group, char* method, char* params);


#endif // _UBUS_INTF_H_ 

