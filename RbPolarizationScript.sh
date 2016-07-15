#!/bin/bash

# script to take faraday scan data and absoption data
# ALL OF THE INFORMATION GATHERED BY THIS SCRIPT IS USED TO CALCUATE NUMBER DENSITY AND Rb POLARIZATION
# Usage:
#
#		sudo ./kensScanScript.sh <aout start> <aout end> <step size> <additional comments>

if [ "$#" -ne 4 ]; then
	echo "usage: sudo ./RbPolarizationScript.sh <aout start> <aout end> <step size> <additional comments>"
else

	# Unblock the beam for sure
	sudo ./setLaserFlag 0

	# set QWP for s+ light
	sudo ./homeWavePlate

	# Take a picture of the chamber
	DATE=$(date +'%Y-%m-%d')
	FILENAME=$(date +'%Y-%m-%d_%H%M%S').jpg
	mkdir -p /home/pi/RbPictures/$DATE
	# take Picture takes the filename and a subfolder to store the 
	# file in within the RbPicture folder
	ssh pi@irpi "/home/pi/karlCode/takePicture.sh $FILENAME $DATE"

	sudo ./setWavePlate 17
	sudo ./faradayscan2 "$1" "$2" "$3" "$4 s+ pump"

	# set QWP for s- light
	sudo ./setWavePlate 105
	sudo ./faradayscan2 "$1" "$2" "$3" "$4 s- pump"

	# set QWP for pi light
	sudo ./setWavePlate 61
	sudo ./faradayscan2 "$1" "$2" "$3" "$4 pi pump"

	#set laser flag to block the pump beam
	sudo ./setLaserFlag 3
	sudo ./faradayscan2 "$1" "$2" "$3" "$4 no beam"

	# set s laser flag to pass the beam
	sudo ./setLaserFlag 0

	# Absorption Scan
	sudo ./RbAbsorbScan 675 975 5 "$4"

fi
