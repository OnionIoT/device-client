#ifndef _UCI_INTF_H_
#define _UCI_INTF_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <onion-debug.h>

#include <device-client-types.h>


int 			uciGet 			(char* option, char* value);

int 			uciSetSection	(char* package, char* section, char* sectionName);
int 			uciSetOption	(char* option, char* value);


#endif // _UCI_INTF_H_ 

