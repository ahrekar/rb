#!/bin/bash

# script to take faraday scan data and absoption data
# ALL OF THE INFORMATION GATHERED BY THIS SCRIPT IS USED TO CALCUATE NUMBER DENSITY AND Rb POLARIZATION
# Usage:
#
#		sudo ./RbPolarizationScript <aout start> <aout end> <step size> <RbScan start> <RbScan end> <additional comments>

if [ "$#" -ne 9 ]; then
	echo "usage: sudo ./RbPolarizationScript.sh <FD aout start> <FD aout end> <FD step size> <RbScan start> <RbScan end> <probeOffset> <magnet1> <magnet2> <additional comments>"
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
	sudo /home/pi/RbControl/faradayScan "$1" "$2" "$3" "$6" "$7" "$8" "$9, no pump"

	# Absorption Scan
	#echo "Running Absorption Scan..."
	#sudo /home/pi/RbControl/RbAbsorbScan "$4" "$5" 5 "$6" "$9"

	# Unblock pump beam for sure
	echo "Unblocking pump beam..."
	sudo /home/pi/RbControl/setLaserFlag 1 0

	#echo "Set magnets to ~10 G"
	#read -p "Input Magnet Voltage 1" magVolt1
	#read -p "Input Magnet Voltage 2" magVolt2

	# set QWP for s+ light
	echo "Setting QWP for S+ light..."
	sudo /home/pi/RbControl/setWavePlate 33
	echo "Faraday Scan: S+ pump..."
	sudo /home/pi/RbControl/faradayScan "$1" "$2" "$3" "$6" "$7" "$8" "$9, s+ pump"
	#sudo /home/pi/RbControl/faradayScan "$1" "$2" "$3" "$6" "$magVolt1" "$magVolt2" "$9, s+ pump"

	# set QWP for s- light
	echo "Setting QWP for S- light..."
	sudo /home/pi/RbControl/setWavePlate 120
	echo "Faraday Scan: S- pump..."
	sudo /home/pi/RbControl/faradayScan "$1" "$2" "$3" "$6" "$7" "$8" "$9, s- pump"
	#sudo /home/pi/RbControl/faradayScan "$1" "$2" "$3" "$6" "$magVolt1" "$magVolt2" "$9, s- pump"
fi
