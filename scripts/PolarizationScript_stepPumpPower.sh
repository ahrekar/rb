#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript_stepPumpPower.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="-160.2"
	N2OFFSET="100"
	N2SWEEP="1.4"
	HEOFFSET=0
	CURRENTSCALE=6
	SCANRANGE=63
	STEPSIZE=24
	DWELL=1
	NUMRUN=1
	DETUNE=1.5
	COMMENTS=$1

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for power in 1800 2300 2800 3300 3800; do 
		echo "About to change current to $power, giving 1 minutes opportunity to cancel" 
		#echo "About to change current to $power, giving 1 minute to cancel." | mutt -s "RbPi Report" karl@huskers.unl.edu
		#sleep 60
		sleep 3

		sudo $RBC/setTACurrent $power
		echo "Giving 30 s for the laser to settle"
  	 	sleep 30
  	 	#sleep 3

		sudo $RBC/scripts/PolarizationScript.sh $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN $DETUNE "power=$power,  $COMMENTS"
	# power LOOP DONE
	done 
	sudo $RBC/setTACurrent 1800
fi
