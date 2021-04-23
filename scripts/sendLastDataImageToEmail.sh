#!/bin/bash

RBC=/home/pi/RbControl

#SYSTEMINFO=$(sudo $RBC/interfacing/mainTemplate)
#IPADDRESS=$(sudo hostname -I)
DATE=$(date +%Y-%m-%d)
DATAFILENAME=$(ls /home/pi/RbData/$DATE/* | tail -n 2)

for i in $DATAFILENAME; do
	rclone copy -v $i box:"Gay Group"/"Project - Rb Spin Filter"/"RbDataRaw"/$DATE
done

