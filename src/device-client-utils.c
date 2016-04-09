#include <device-client-utils.h>

void jsonPrint (int severity, json_object *jobj, char* tab)
{
	enum json_type type;
	char 	tabNext[STRING_LENGTH];

	json_object_object_foreach(jobj, key, val) {
		onionPrint(severity, "%s%s : ", tab, key);
		type = json_object_get_type(val);

		switch (type) {
			case json_type_null: 
				onionPrint(severity, "null\n");
				break;
			case json_type_boolean: 
			    onionPrint(severity, "%d (bool)\n", json_object_get_boolean(val) );
			    break;
			case json_type_double:
			    onionPrint(severity, "%lf (double)\n", json_object_get_double(val) );
			    break;
			case json_type_int:
			    onionPrint(severity, "%d (int)\n", json_object_get_int(val) );
			    break;
			case json_type_string:
			    onionPrint(severity, "\"%s\"\n", json_object_get_string(val) );
			    break;
			case json_type_object:
				// increment the tab
				sprintf(tabNext, "%s\t", tab);

				onionPrint(severity, "\n");
				jobj = json_object_object_get(jobj, key);
				jsonPrint(severity, jobj, tabNext);
				break;
		}
	}
}

/*int jsonFindByKey (char* key, json_object *jobj, json_object *jret)
{
	int 	status;

	enum json_type type;

	status 	= json_object_object_get_ex(jobj, key, jret);
	json_object_object_foreach(jobj, keyCurrent, val) {
		if (strncmp(key, keyCurrent, strlen(key)) == 0 ) {
			jret 	= 
		}
	}
}*/

// populates 'value' with string from json object
// 	returns:
//	EXIT_SUCESS 	- valid string populated into value
// 	EXIT_FAILURE	- object is not of type string
int jsonGetString (json_object *jobj, char* value)
{
	int 	status;
	enum json_type type;

	// find the type
	type = json_object_get_type(jobj);

	if (type == json_type_string) {
		status 	= EXIT_SUCCESS;
		strcpy(value, json_object_get_string(jobj) );
	}
	else {
		status 	= EXIT_FAILURE;
	}

	return 	status;
}

// returns:
//	1	- if the text is json
//	0	- if not
int isJson (char* text, int size)
{
	int bJson 	= 0;

	if (size > 0 && text[0] == '{' && text[size-1] == '}') {
		bJson 	= 1;
	}

	return bJson;
}

