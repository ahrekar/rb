#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="130.0"
	N2OFFSET="49.1"
	N2SWEEP="12.8"
	HEOFFSET=-10
	CURRENTSCALE=7
	SCANRANGE=30
	STEPSIZE=24
	DWELL=1
	NUMRUN=4
	COMMENTS=$1

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for power in 2500 2300 2000 1500 1000 500 0; do 
		echo "About to change current to $power, giving 1 minutes opportunity to cancel" 
		echo "About to change current to $power" | mutt -s "RbPi Report" karl@huskers.unl.edu
		sleep 60

		sudo $RBC/setTACurrent $power
		echo "Giving 30 s for the laser to settle"
  	 	sleep 30

		sudo $RBC/scripts/PolarizationScript.sh $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN "power=$power,  $COMMENTS"
	# power LOOP DONE
	done 
	echo "Finished with pump power scan run." | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
