#!/bin/bash

# script to take faraday scan data and absoption data
# ALL OF THE INFORMATION GATHERED BY THIS SCRIPT IS USED TO CALCUATE NUMBER DENSITY AND Rb POLARIZATION
# Usage:
#
#		sudo ./RbPolarizationScript <aout start> <aout end> <step size> <RbScan start> <RbScan end> <additional comments>

if [ "$#" -ne 4 ]; then
	echo "usage: sudo ./RbPolarizationScript.sh <probeOffset> <magnet1> <magnet2> <additional comments>"
else

	# Take a picture of the chamber
	DATE=$(date +'%Y-%m-%d')
	FILENAME=$(date +'%Y-%m-%d_%H%M%S').jpg
#	mkdir -p /home/pi/RbPictures/$DATE
	# take Picture takes the filename and a subfolder to store the 
	# file in within the RbPicture folder
#	ssh pi@irpi "/home/pi/karlCode/takeRemotePicture.sh $FILENAME $DATE"

	#set laser flag to block the pump beam
	echo "Blocking Pump Beam..."
	sudo /home/pi/RbControl/setLaserFlag 1 1
	echo "Faraday Scan: no pump..."
	sudo /home/pi/RbControl/faradayScan "$1" "$2" "$3" "$4, no pump"

	# Unblock pump beam for sure
	echo "Unblocking pump beam..."
	sudo /home/pi/RbControl/setLaserFlag 1 0
fi
