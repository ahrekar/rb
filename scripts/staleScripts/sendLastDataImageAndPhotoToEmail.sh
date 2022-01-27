#!/bin/bash

RBC=/home/pi/RbControl

SYSTEMINFO=$(sudo $RBC/interfacing/mainTemplate)
IPADDRESS=$(sudo hostname -I)
PICFILENAME=$(ls /home/pi/RbPictures/$(date +%Y-%m-%d)* | tail -n 1)
DATAFILENAME=$(ls /home/pi/RbData/$(date +%Y-%m-%d)/*.png | tail -n 1)

echo "$SYSTEMINFO" | mutt -s "RbPi ($IPADDRESS) Report" -a "$DATAFILENAME" "$PICFILENAME" -- karl@huskers.unl.edu
