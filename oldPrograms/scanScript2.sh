#!/bin/bash

# script to take faraday scan data
# WITHOUT pump 
# Usage:
#
#		sudo ./scanScript2.sh <aout start> <aout end> <step size> <additional comments>

if [ "$#" -ne 4 ]; then
	echo "usage: sudo ./scanScript2.sh <aout start> <aout end> <step size> <additional comments>"
else

	#set laser flag to block the pump beam
	sudo ./setLaserFlag 3
	sudo ./faradayscan2 "$1" "$2" "$3" "$4 no beam"
	# Absorption Scan
	sudo ./RbAbsorbScan 600 1000 10 "$4"

	# set s laser flag to pass the beam
	sudo ./setLaserFlag 0
fi
