#!/bin/bash
# Script to obtain polarization Data

if [ "$#" -ne 4 ]; then
	echo "usage: sudo ./faradayAndPolarization.sh <aout start> <aout end> <step size> <additional comments>"
else
	# set QWP for s+ light
	sudo ./setWavePlate 17
	sudo ./faradayscan2 "$1" "$2" "$3" "$4 s+ pump"
	sudo ./polarization 100 1 "$4 s+ pump"

	# set QWP for s- light
	sudo ./setWavePlate 105
	sudo ./faradayscan2 "$1" "$2" "$3" "$4 s- pump"
	sudo ./polarization 100 1 "$4 s- pump"
fi
