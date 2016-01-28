# device-client
Omega client for communication with device server

## Testing

### To Build the Current Code
Run `sh ./tools/remote_compile.sh` from your local repo.

This will perform the following:
* Upload your local code to the openwrt2.onion.io server
* Cross Compile
* Create an ipk package


### Installation on the Omega
First copy `./tools/install-device-client.sh` to your target Omega.

On the Omega, run `sh install-device-client.sh` to grab the latest device-client ipk from the openwrt2.onion.io server.

### Running the Program
Once the ipk is installed on the Omega, just do `device-client` to run the program.

