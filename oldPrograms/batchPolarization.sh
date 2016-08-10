#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "usage: sudo ./dataCollectionScript.sh <dwell time> <comments>"
else
	for i in $(seq 1 10); do 
		# measure background polarization
		sudo /home/pi/RbControl/polarization 400 "$1" "$2, background, Run $i"

		# measure polarization
		sudo /home/pi/RbControl/polarization 0 "$1" "$2, exciting He, Run $i"
	done
fi
