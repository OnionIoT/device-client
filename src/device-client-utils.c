#include <device-client-utils.h>

void jsonPrint (int severity, json_object *jobj)
{
	enum json_type type;

	json_object_object_foreach(jobj, key, val) {
		onionPrint(severity, "%s : ", key);
		type = json_object_get_type(val);

		switch (type) {
			case json_type_null: 
				onionPrint(severity, "null\n");
				break;
			case json_type_boolean: 
			    onionPrint(severity, "%d (bool)\n", json_object_get_boolean(val));
			    break;
			case json_type_double:
			    onionPrint(severity, "%lf (double)\n", json_object_get_double(val));
			    break;
			case json_type_int:
			    onionPrint(severity, "%d (int)\n", json_object_get_int(val));
			    break;
			case json_type_object:
			    jobj = json_object_object_get(jobj, key);
			    jsonPrint(severity, jobj);
			    break;
			case json_type_string:
			    onionPrint(severity, "\"%s\"\n", json_object_get_string(val));
			    break;
		}
	}
}

int jsonFindByKey (char* key, json_object *jobj, json_object *jret)
{
	int 	status;

	enum json_type type;

	status 	= json_object_object_get_ex(jobj, key, jret);
	/*json_object_object_foreach(jobj, keyCurrent, val) {
		if (strncmp(key, keyCurrent, strlen(key)) == 0 ) {
			jret 	= 
		}
	}*/
}