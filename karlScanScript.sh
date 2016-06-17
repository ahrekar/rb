#!/bin/bash

# This is a script to analyze the effect that the magnetic
# field has on the plane of polarization. It will do the 
# faraday rotation analysis for a single detuning of the
# probe laser. Data will be taken for both when the pump
# entering the chamber and when it is not.

sudo ./setOmega 140
sudo ./waitForOmega 140
for i in $(seq 0 .1 3); do 
	sudo ./setBKamps $i
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
	sudo ./faradayscan2 0 1000 200 "Coil Current at: $i, Pump Wavelength: 377.1057 (28.120), s+ pump, $1"

	# set QWP for s- light
	sudo ./setWavePlate 105
	sudo ./faradayscan2 0 1000 200 "Coil Current at: $i, Pump Wavelength: 377.1057 (28.120), s- pump, $1"

	# set QWP for pi light
	sudo ./setWavePlate 61
	sudo ./faradayscan2 0 1000 200 "Coil Current at: $i, Pump Wavelength: 377.1057 (28.120), pi pump, $1"

	#set laser flag to block the pump beam
	sudo ./setLaserFlag 3
	sudo ./faradayscan2 0 1000 200 "Coil Current at: $i, Pump Wavelength: 377.1057 (28.120), no beam, $1"

	# set s laser flag to pass the beam
	sudo ./setLaserFlag 0

	# Absorption Scan
	sudo ./RbAbsorbScan 575 975 2 "$1"
done
sudo ./setOmega 17
sudo ./setBKamps 0.0
