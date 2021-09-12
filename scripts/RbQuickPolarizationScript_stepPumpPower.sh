#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./RbPolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	NUMRUN=1
	COMMENTS=$1

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	#for power in 3800 3500 3200 2900 2600 2300 2000 1800; do 
	for power in 1800 3500 2000 3200 2300 2900 2600; do 
		echo "About to change current to $power, giving 15 seconds opportunity to cancel" 
		sleep 15

		sudo $RBC/setTACurrent $power
		echo "Giving 30 s for the laser to settle"
  	 	sleep 30

		sudo $RBC/setPumpDetuning 1.5

		sudo ./RbQuickPolarizationScript.sh "current->$power, $COMMENTS"
	# power LOOP DONE
	done 
fi
