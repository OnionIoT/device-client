#!/bin/sh

cd /tmp 

rm -rf *.ipk

wget "http://openwrt2.onion.io:8001/device-client_0.1-1_ar71xx.ipk"

opkg install device-client_0.1-1_ar71xx.ipk

