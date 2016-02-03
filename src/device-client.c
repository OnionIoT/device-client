#include <device-client.h>

int deviceClientOperation (json_object *jobj)
{
	int 			status 	= EXIT_FAILURE;
	json_object 	*jGroup;
	json_object 	*jMethod;
	char			group[STRING_LENGTH];
	char			method[STRING_LENGTH];

	jsonPrint(ONION_SEVERITY_DEBUG, jobj);

	// check the received json for the 'group' and 'method' objects
	status 	= 	json_object_object_get_ex(jobj, JSON_REQUEST_GROUP_KEY, &jGroup);
	status 	|=	json_object_object_get_ex(jobj, JSON_REQUEST_METHOD_KEY, &jMethod);

	// check if the 'group' and 'method' object are both found
	if (status != 0) {
		onionPrint(ONION_SEVERITY_DEBUG, ">> Found 'group' and 'method' objects\n"); 

		// read the strings from the objects
		status	= 	jsonGetString(jGroup, &group);
		status	|= 	jsonGetString(jMethod, &method);

		// check that strings were read properly
		if (status == EXIT_SUCCESS) {
			onionPrint(ONION_SEVERITY_INFO, ">> Received command request for '%s' group, '%s' function\n", group, method);

			// LAZAR: add ubus call

			// reply to server
		}
	}
	

	return 	status;
}

