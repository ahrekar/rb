#!/bin/bash

RBC=/home/pi/RbControl

SYSTEMINFO=$(sudo $RBC/interfacing/mainTemplate)
IPADDRESS=$(sudo hostname -I)
DATAFILENAME=$(ls /home/pi/RbData/$(date +%Y-%m-%d)/*.png | tail -n 1)

echo "$SYSTEMINFO" | mutt -s "RbPi($IPADDRESS)" -a $DATAFILENAME -- karl@huskers.unl.edu
