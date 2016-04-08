#!/bin/sh

# include the Onion sh lib
. /usr/lib/onion/lib.sh


###########################
##### Globals         #####
# constants
magicUrl="https://api.onion.io/util/cloud/snapson"
magicLocal="/tmp/magic"

# options
bVerbose=0
bJson=0

# commands
bSetup=0

# variables
value=""



#############################
##### Print Usage ###########
usage () {
	echo "Functionality:"
	echo "	Configure the device to connect to the Onion Cloud using a setup code"
	echo ""
	echo "Usage: $0 setup <setup code>"
	echo ""
	
	echo ""
}


# function to setup the Omega for the Onion Cloud: set the device ID and secret
# 	$1 	- magic key
_SetupCloud () {
	magicKey="$1"
	echo "> Running Onion Cloud setup with setup code: $magicKey"

	# download the magic file
	rm -rf $magicLocal >& /dev/null
	dl=$(DownloadUrl "$magicUrl/$magicKey" "$magicLocal")

	# read the magic file json
	if [ $dl == 0 ]; then
		# parse the json file
		local RESP="$(cat $magicLocal)"
		json_load "$RESP"

		json_get_var jError error
		json_get_var jId deviceId
		json_get_var jSecret secret

		if [ "$jError" == "expired" ]; then
			echo "ERROR: setup code has expired, please generate a new setup code on the Cloud"
		elif [ "$jError" == "not found" ]; then
			echo "ERROR: setup code is invalid!"
		else
			# setup the onion.cloud uci configuration
			# add the device info to the onion config
			ubus call uci add '{"config":"onion", "type":"cloud", "name":"cloud"}' >& /dev/null
			ubus call uci set '{"config":"onion", "section":"cloud", "values":{"deviceId":"'"$jId"'", "secret":"'"$jSecret"'"}}'
			ubus call uci commit '{"config":"onion"}'

			/etc/init.d/device-client restart  >& /dev/null
			echo "> Device is now setup for the Onion Cloud"
		fi
	fi
}



###########################
##### Parse Arguments #####

# parse arguments
while [ "$1" != "" ]
do
	case "$1" in
		# options
		-v|--v|-verbose|verbose)
			bVerbose=1
			shift
		;;
		-j|--j|-json|--json|json)
			bJson=1
			shift
		;;
		# commands
    	-s|--s|setup|-setup|--setup)
			bSetup=1
			shift
			value="$1"
			shift
		;;
	    *)
			echo "ERROR: Invalid Argument: $1"
			usage
			exit
		;;
	esac
done



########################
########################
##### Main Program #####
## commands
if [ $bSetup == 1 ]; then
	# perform the cloud setup
	_SetupCloud $value
else 
	usage
fi



