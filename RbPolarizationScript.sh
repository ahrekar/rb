#!/bin/bash

# script to take faraday scan data and absoption data
# ALL OF THE INFORMATION GATHERED BY THIS SCRIPT IS USED TO CALCUATE NUMBER DENSITY AND Rb POLARIZATION
# Usage:
#
#		sudo ./RbPolarizationScript <aout start> <aout end> <step size> <RbScan start> <RbScan end> <additional comments>

if [ "$#" -ne 6 ]; then
	echo "usage: sudo ./RbPolarizationScript.sh <aout start> <aout end> <step size> <RbScan start> <RbScan end> <additional comments>"
else
	# Unblock the beam for sure
	sudo /home/pi/RbControl/setLaserFlag 1 0

	# Take a picture of the chamber
	DATE=$(date +'%Y-%m-%d')
	FILENAME=$(date +'%Y-%m-%d_%H%M%S').jpg
#	mkdir -p /home/pi/RbPictures/$DATE
	# take Picture takes the filename and a subfolder to store the 
	# file in within the RbPicture folder
#	ssh pi@irpi "/home/pi/karlCode/takeRemotePicture.sh $FILENAME $DATE"

	# set QWP for s+ light
	sudo /home/pi/RbControl/setWavePlate 17
	sudo /home/pi/RbControl/faradayscan "$1" "$2" "$3" "$6, s+ pump"

	# set QWP for s- light
	sudo /home/pi/RbControl/setWavePlate 105
	sudo /home/pi/RbControl/faradayscan "$1" "$2" "$3" "$6, s- pump"

	#set laser flag to block the pump beam
	sudo /home/pi/RbControl/setLaserFlag 1 1
	sudo /home/pi/RbControl/faradayscan "$1" "$2" "$3" "$6, no beam"

	# Absorption Scan
	sudo /home/pi/RbControl/RbAbsorbScan "$4" "$5" 5 "$6"

	# Unblock the beam
	sudo /home/pi/RbControl/setLaserFlag 1 0
fi
