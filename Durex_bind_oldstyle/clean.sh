#!/bin/bash

SERV_NAME="durex"
DUREX_PID=$(pidof durex)

kill ${DUREX_PID}
update-rc.d durex remove
rm -f /etc/init.d/durex
rm -f /bin/durex
systemctl daemon-reload
