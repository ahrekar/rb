#!/bin/bash
# Script to obtain polarization Data

if [ "$#" -ne 4 ]; then
	echo "usage: sudo ./faradayAndPolarization.sh <aout start> <aout end> <step size> <additional comments>"
else
	# set QWP for s+ light
	sudo ./setWavePlate 17
	# Run a faraday scan
	sudo ./faradayscan2 "$1" "$2" "$3" "$4 s+ pump"
	# Make sure the polarization plate will start in home
	sudo ./homemotor 0
	# Run the polarization code.
	sudo ./polarization 100 1 "$4 s+ pump"

	# set QWP for s- light
	sudo ./setWavePlate 105
	# Run Faraday Scan
	sudo ./faradayscan2 "$1" "$2" "$3" "$4 s- pump"
	# Make sure the polarimeter is in the home position
	sudo ./homemotor 0
	# Run the polarization code.
	sudo ./polarization 100 1 "$4 s- pump"
fi
