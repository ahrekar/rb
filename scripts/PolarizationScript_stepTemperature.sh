#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="120.0"
	N2OFFSET="37.3"
	N2SWEEP="5.8"
	HEOFFSET=40
	CURRENTSCALE=6
	SCANRANGE=30
	STEPSIZE=24
	DWELL=2
	NUMRUN=7
	NUMEQUILRUN=30
	COMMENTS=$1
	STARTTEMP=121
	ENDTEMP=136
	STEPTEMP=3

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for temp in $(seq $STARTTEMP $STEPTEMP $ENDTEMP); do 
		echo "About to change temperature to $temp, giving 5 minutes opportunity to cancel" 
		echo "About to change temperature to $temp" | mutt -s "RbPi Report" karl@huskers.unl.edu
		sleep 300
		sudo $RBC/setOmega 150 $temp

		sudo $RBC/scripts/repeatRbQuickPolarization.sh $NUMEQUILRUN "Equil runs for temp=$temp (from $STARTTEMP to $ENDTEMP in steps of $STEPTEMP), $COMMENTS"
		sudo $RBC/scripts/PolarizationScript.sh $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN "temp=$temp,  $COMMENTS"
	# TEMP LOOP DONE
	done 
fi
