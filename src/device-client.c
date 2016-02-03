#include <device-client.h>

int deviceClientOperation (json_object *jobj)
{
	int 	status;
	json_object 	*jret;

	status 	= json_object_object_get_ex(jobj, "group", NULL);	//fix last arg

	// check if object with key 'group' is found
	if (status != 0) {
		// found the a group object
		onionPrint(ONION_SEVERITY_DEBUG, "found 'group' object (status: %d)\n", status); 
	}
	

	return 	status;
}

