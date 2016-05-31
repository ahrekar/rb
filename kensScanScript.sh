#!/bin/bash

# script to take faraday scan data
# Usage:
#
#		sudo ./kensScanScript.sh <aout start> <aout end> <step size> <additional comments>

if [ "$#" -ne 4 ]; then
	echo "usage: sudo ./kensScanScript.sh <aout start> <aout end> <step size> <additional comments>"
else

	# Unblock the beam for sure
	sudo ./setLaserFlag 0

	# set QWP for s+ light
	sudo ./homeWavePlate
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
	# Absorption Scan
	sudo ./RbAbsorbScan 600 1000 10 "$4"

	# set s laser flag to pass the beam
	sudo ./setLaserFlag 0
fi
