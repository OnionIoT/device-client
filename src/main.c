#include <main.h>

void usage(const char* progName) 
{
	printf("device-client: interface with Onion cloud device-server\n");
}

int main(int argc, char** argv)
{
	const char 	*progname;
	int 		status;
	int 		ch;
	int 		verbose, debug;

	int 		bRun;
	char		*deviceId, *key;
	char 		*host;

	// set defaults
	verbose 		= ONION_VERBOSITY_NORMAL;
	debug 			= 0;

	// save the program name
	progname 		= argv[0];	


	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqdh")) != -1) {
		switch (ch) {
		case 'v':
			// verbose output
			verbose++;
			break;
		case 'q':
			// quiet output
			verbose = ONION_SEVERITY_FATAL;
			break;
		case 'd':
			// debug mode
			debug++;
			break;
		default:
			usage(progname);
			return 0;
		}
	}

	// advance past the option arguments
	//argc 	-= optind;
	//argv	+= optind;


	// debugging options
	host 	= malloc(STRING_LENGTH * sizeof *host);
	if (debug < DC_DEBUG_LEVEL_SERVER) {
		strncpy(host, DEVICE_SERVER, strlen(DEVICE_SERVER) );
	}
	else {
		strncpy(host, DEBUG_SERVER, strlen(DEBUG_SERVER) );
	}


	///////////////////
	//* initialization *//
	// set verbosity
	onionSetVerbosity(verbose);
	// curl init
	curlInit();
	// allocate memory
	deviceId 	= malloc(STRING_LENGTH * sizeof *deviceId);
	key 	 	= malloc(STRING_LENGTH * sizeof *key);


	//* program *//
	// find the device id and key
	status	= dcGetIdentity(deviceId, key);
	// setup the device client
	status 	|= dcSetup(deviceId, key, host);
	// Lazar - potentially move the frees for deviceId and key here

	// launch the device client
	bRun 	= 1;
	if (status == EXIT_SUCCESS) {
		while (bRun) {
			status 	= dcRun(debug);
			onionPrint(ONION_SEVERITY_INFO, "  > Restarting connection...\n");
			sleep(5);
		}
	}
	else {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: Could not read device ID and secret key!\n");
	}

	//* clean-up *//
	free(host);
	free(deviceId);
	free(key);
	// curl cleanup
	curlCleanup();
	
	return 0;
}

