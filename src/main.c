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
			debug 	= 1;
			break;
		default:
			usage(progname);
			return 0;
		}
	}

	// advance past the option arguments
	//argc 	-= optind;
	//argv	+= optind;


	// debug options
	host 	= malloc(STRING_LENGTH * sizeof *host);
	if (debug == 0) {
		strncpy(host, DEVICE_SERVER, strlen(DEVICE_SERVER) );
	}
	else {
		strncpy(host, DEBUG_SERVER, strlen(DEBUG_SERVER) );
	}


	///////////////////
	// set verbosity
	onionSetVerbosity(verbose);

	// curl init
	curlInit();

	// curl listen
	status 	= dsListen("dev1","key1", host);
	
	// curl cleanup
	curlCleanup();
	

	return 0;
}

