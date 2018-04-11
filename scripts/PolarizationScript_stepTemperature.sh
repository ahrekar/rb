#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="120.0"
	N2OFFSET="2"
	N2SWEEP="4.6"
	HEOFFSET=80
	CURRENTSCALE=8
	SCANRANGE=30
	STEPSIZE=24
	DWELL=1
	NUMRUN=15
	NUMEQUILRUN=30
	COMMENTS=$1
	CCELLTEMP=180
	STARTTEMP=125
	ENDTEMP=100
	STEPTEMP=-2

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for temp in $(seq $STARTTEMP $STEPTEMP $ENDTEMP); do 
		echo "About to change temperature to $temp, giving 5 minutes opportunity to cancel" 
		echo "About to change temperature to $temp" | mutt -s "RbPi Report" karl@huskers.unl.edu
		sleep 300
		sudo $RBC/setOmega $CCELLTEMP $temp

		sudo $RBC/scripts/repeatRbQuickPolarization.sh $NUMEQUILRUN "Equil runs for temp=$temp (from $STARTTEMP to $ENDTEMP in steps of $STEPTEMP), $COMMENTS"
		sudo $RBC/scripts/PolarizationScript.sh $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN "temp=$temp,  $COMMENTS"
	# TEMP LOOP DONE
	done 
	echo "Done with temp step run from $STARTTEMP to $ENDTEMP" | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
