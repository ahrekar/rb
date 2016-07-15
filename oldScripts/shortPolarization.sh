#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "usage: sudo ./dataCollectionScript.sh <dwell time> <comments>"
else
	# measure background polarization
	for i in $(seq 1 20); do 
		sudo ./polarization 500 "$1" "$2, background"
		# set QWP for s+ light
		#sudo ./setWavePlate 17
		# Run the polarization code.
		sudo ./polarization 0 "$1" "$2, no pump"
	done

	## set QWP for s- light
	#sudo ./setWavePlate 105
	## Run the polarization code.
	#sudo ./polarization 0 "$1" "$2, s- pump"

	#sudo ./homeWavePlate
fi
