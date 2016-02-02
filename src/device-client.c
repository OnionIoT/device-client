#include <device-client.h>

void usage(const char* progName) 
{
	printf("device-client: interface with Onion cloud device-server\n");
}

int main(int argc, char** argv)
{
	const char 	*progname;
	int 		status;
	int 		ch;
	int 		verbose;

	// set defaults
	verbose 		= ONION_VERBOSITY_NORMAL;

	// save the program name
	progname 		= argv[0];	


	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqh")) != -1) {
		switch (ch) {
		case 'v':
			// verbose output
			verbose++;
			break;
		case 'q':
			// quiet output
			verbose = ONION_SEVERITY_FATAL;
			break;
		default:
			usage(progname);
			return 0;
		}
	}

	// advance past the option arguments
	//argc 	-= optind;
	//argv	+= optind;


	///////////////////
	// set verbosity
	onionSetVerbosity(verbose);

	// curl init
	curlInit();

	// curl listen
	status 	= dsListen();
	
	// curl cleanup
	curlCleanup();
	
	printf(DEVICE_SERVER);

	return 0;
}

