#!/bin/sh /etc/rc.common
# Copyright (C) 2016 Onion Corporation
START=60

BIN="device-client"
USE_PROCD=1

start_service() {
        procd_open_instance
        procd_set_param command $BIN
        procd_set_param respawn
        procd_close_instance
}


