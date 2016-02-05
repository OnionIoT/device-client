#include <uci-intf.h>
#include "uci.h"

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
		if (ptr.target == UCI_TYPE_OPTION) {
			strncpy(value, ptr.o->v.string, strlen(ptr.o->v.string) );
			onionPrint(ONION_SEVERITY_DEBUG_EXTRA, ">> uci get '%s' success: '%s'\n", option, value);
			status 	= EXIT_SUCCESS;
		}
	}

	// clean-up
	uci_free_context (ctx);

	return 	status;
}