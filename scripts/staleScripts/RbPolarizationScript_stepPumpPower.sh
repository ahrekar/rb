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

	for power in 3500 2500 1500 1250; do 
		echo "About to change current to $power, giving 1 minutes opportunity to cancel" 
		sleep 60

		sudo $RBC/setTACurrent $power
		echo "Giving 30 s for the laser to settle"
  	 	sleep 30

		for run in $(seq $NUMRUN); do 
			sudo ./RbPolarizationScript.sh "current->$power, run->$run, totalRuns->$NUMRUN, $COMMENTS"
		#RUN LOOP DONE
		done 
	# power LOOP DONE
	done 
fi
