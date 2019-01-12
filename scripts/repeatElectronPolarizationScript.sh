#!/bin/bash

RBC=/home/pi/RbControl

if [ "$#" -ne 2 ]; then
	echo "usage: ./repeatElectronPolarization.sh <number of runs> <comments>"
	echo "                                                  "
else
	for i in $(seq $1); do
		echo "Run number ${i}/$1, pausing for 3 seconds to allow the opportunity to cancel"
		sleep 3
		sudo $RBC/scripts/ElectronPolarizationScript.sh 240 1 6 "A few polarized runs so I can feel like I'm doing something as I try to organize data and my life NOW at 1700 mA on the Tapered ampilfier current. Run${i}/$1"
	done
fi
