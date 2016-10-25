#!/bin/bash

# script to take faraday scan data and absoption data
# ALL OF THE INFORMATION GATHERED BY THIS SCRIPT IS USED TO CALCUATE NUMBER DENSITY AND Rb POLARIZATION
# Usage:
#
#		sudo ./RbPolarizationScript <aout start> <aout end> <step size> <RbScan start> <RbScan end> <additional comments>

if [ "$#" -ne 6 ]; then
	echo "usage: sudo ./RbPolarizationScript.sh <aout start> <aout end> <step size> <RbScan start> <RbScan end> <additional comments>"
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
	sudo /home/pi/RbControl/faradayscan "$1" "$2" "$3" "$6, no beam"

	# Absorption Scan
	echo "Running Absorption Scan..."
	sudo /home/pi/RbControl/RbAbsorbScan "$4" "$5" 5 "$6"

	# Unblock pump beam for sure
	echo "Unblocking pump beam..."
	sudo /home/pi/RbControl/setLaserFlag 1 0

	# set QWP for s+ light
	echo "Setting QWP for S+ light..."
	sudo /home/pi/RbControl/setWavePlate 17
	echo "Faraday Scan: S+ pump..."
	sudo /home/pi/RbControl/faradayscan "$1" "$2" "$3" "$6, s+ pump"

	# set QWP for s- light
	echo "Setting QWP for S- light..."
	sudo /home/pi/RbControl/setWavePlate 105
	echo "Faraday Scan: S- pump..."
	sudo /home/pi/RbControl/faradayscan "$1" "$2" "$3" "$6, s- pump"
fi
