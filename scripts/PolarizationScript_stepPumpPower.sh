#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="-129.4"
	N2OFFSET="60.0"
	N2SWEEP="20.9"
	HEOFFSET=-20
	CURRENTSCALE=6
	SCANRANGE=63
	STEPSIZE=16
	DWELL=1
	NUMRUN=7
	COMMENTS=$1

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for power in 3500 2500 1500 1250; do 
		echo "About to change current to $power, giving 1 minutes opportunity to cancel" 
		echo "About to change current to $power, giving 1 minute to cancel." | mutt -s "RbPi Report" karl@huskers.unl.edu
		sleep 60
		#sleep 3

		sudo $RBC/setTACurrent $power
		echo "Giving 30 s for the laser to settle"
  	 	sleep 30
  	 	#sleep 3

		sudo $RBC/scripts/PolarizationScript.sh $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN "power=$power,  $COMMENTS"
	# power LOOP DONE
	done 
	echo "Finished with pump power scan run." | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
