#ifndef _CURL_COMMUNICATION_H_
#define _CURL_COMMUNICATION_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <onion-debug.h>

#include "curl/curl.h"
#include "json-c/json.h"


int 			curlInit		();
int 			curlCleanup		();


int 			dsListen 		();

#endif // _CURL_COMMUNICATION_H_ 

