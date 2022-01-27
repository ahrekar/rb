#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./stepPumpPower.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"

	for power in 0 300 600 900 1200 1500 1800 2100 2200 2300 2400 2500; do 
		echo "About to change current to $power"
		sleep 3

		sudo $RBC/setTACurrent $power
		echo "Giving 5 s for the laser to settle, recording power"
  	 	sleep 5

	# power LOOP DONE
	done 
fi
