#include <uci-intf.h>
#include "uci.h"

// function prototypes
int 	_uciSet		(char* option, char* value, int uciType, bool bCommit);


// get an option value
int uciGet(char* option, char* value)
{
	int 	status	= EXIT_FAILURE;
	struct 	uci_context 		*ctx;
	struct 	uci_ptr 			ptr;

	// initialize uci context
	ctx 	= uci_alloc_context();

	if (!ctx) {
		return EXIT_FAILURE;
	}

	// uci look-up
	if ( uci_lookup_ptr(ctx, &ptr, option, true) == UCI_OK ) {
		// get the option value
		if (ptr.target == UCI_TYPE_OPTION && ptr.o != NULL) {
			strncpy(value, ptr.o->v.string, strlen(ptr.o->v.string) );
			onionPrint(ONION_SEVERITY_DEBUG_EXTRA, ">> uci get '%s' success: '%s'\n", option, value);
			status 	= EXIT_SUCCESS;
		}
	}

	// clean-up
	uci_free_context (ctx);

	return 	status;
}

// add an unnamed uci section
// returns sectionName which will contain the generated name
int uciAddSection(char* package, char* section, char* sectionName)
{
	int 	status;
	struct 	uci_context 		*ctx;
	struct 	uci_package 		*p = NULL;
	struct 	uci_section 		*s = NULL;
	
	// initialize uci context
	ctx 	= uci_alloc_context();

	if (!ctx) {
		return EXIT_FAILURE;
	}

	// load the package
	status 	= uci_load(ctx, package, &p);	
	if (status == UCI_OK) {
		// add the section
		status 	= uci_add_section(ctx, p, section, &s);
		if (status == UCI_OK) {
			// save the package changes
			uci_save(ctx, p);
		}
	}

	// copy the cfg name of the new section
	strcpy(sectionName, s->e.name);

	// clean-up
	uci_free_context (ctx);

	return	(status == UCI_OK ? EXIT_SUCCESS : EXIT_FAILURE);
}

// add a named uci section
int uciSetSection(char* package, char* section, char* sectionName)
{
	int 	status;
	char* 	option;
	char 	givenSectionName[STRING_LENGTH];

	//	printf("given section name: '%s'\n", givenSectionName);
	option 	= malloc(STRING_LENGTH * sizeof *option);

	sprintf(option, "%s.%s=%s", package, section, sectionName);
	onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "Setting section '%s' to '%s'\n", option, sectionName);
	status = _uciSet(option, sectionName, UCI_TYPE_SECTION, true);

	// clean-up
	free(option);

	return status;
}

// add/set a uci option
int uciSetOption(char* option, char* value)
{
	int 	status;

	status = _uciSet(option, value, UCI_TYPE_OPTION, true);

	return status;
}

// set a specified type
int _uciSet(char* option, char* value, int uciType, bool bCommit)
{
	int 	status;
	struct 	uci_context 		*ctx;
	struct 	uci_ptr 			ptr;

	// initialize uci context
	ctx 	= uci_alloc_context();

	if (!ctx) {
		onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "_uciSet: failed allocating context\n");
		return EXIT_FAILURE;
	}

	// perform the lookup
	status 	= uci_lookup_ptr(ctx, &ptr, option, (bool)true);
	if ( status != UCI_OK ) {
		onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "_uciSet: could not find option '%s', returned %d\n", option, status);
		status 	= EXIT_FAILURE;
	}

	// set the option value
	if (uciType == UCI_TYPE_OPTION && ptr.target == UCI_TYPE_OPTION) {
		ptr.value 	= value;
		status 		= EXIT_SUCCESS;
		
		// set the change
		if ((uci_set(ctx, &ptr) != UCI_OK) || (ptr.o==NULL || ptr.o->v.string==NULL)) {
			onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "_uciSet:OPTION:: failed when setting the change\n");
			status 	= EXIT_FAILURE;
		}
	}
	else if (uciType == UCI_TYPE_SECTION) {
		status = uci_set(ctx, &ptr);
		onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "_uciSet:SECTION:: set returned %d\n", status); 
	}

	if (bCommit) {
		// commit the change
		status 	= uci_commit(ctx, &ptr.p, false);
		onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "_uciSet: uci commit command returned %d\n", status);
	}

	// clean-up
	uci_free_context (ctx);

	return 	status;
}




