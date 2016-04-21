#!/bin/sh

## define the remote server and package
server="lazar@build.onion.io"
remotePath="/home/lazar/OpenWRT-Buildroot-x86/openwrt/dl"
package="device-client"
flags="CONFIG_DEBUG=y TARGET_OPTIMIZATION='-ggdb3 -O0' STRIP='/bin/true'"
#flags="CONFIG_DEBUG=y STRIP='/bin/true'"

localPath="../$package"

## upload project to remote server
cmd="rsync -va --progress $localPath $server:$remotePath"
echo "$cmd"
eval "$cmd"


## create a tar from the file, run the compile
cmd="ssh $server \"cd $remotePath && tar -zcvf $package.tar.gz $package && cd .. && make package/$package/compile V=99 $flags\""
echo "$cmd"
eval "$cmd"

