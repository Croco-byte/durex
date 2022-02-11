#!/bin/bash

SERV_NAME="durex"
SYSD_LOC="$(pkg-config --variable=systemdsystemunitdir systemd)"

systemctl stop ${SERV_NAME}
systemctl disable ${SERV_NAME}
rm ${SYSD_LOC}/${SERV_NAME}.service
rm /etc/systemd/system/${SERV_NAME}.service
systemctl daemon-reload
systemctl reset-failed
rm -f /bin/durex
