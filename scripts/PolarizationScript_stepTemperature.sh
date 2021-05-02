#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="-130.0"
	N2OFFSET="71.6"
	N2SWEEP="3.0"
	HEOFFSET=-19.9
	CURRENTSCALE=6
	SCANRANGE=30
	STEPSIZE=24
	DWELL=1
	NUMRUN=3
	DETUNE=1.5
	NUMEQUILRUN=5
	COMMENTS=$1
	CCELLTEMP=140
	STARTTEMP=90
	ENDTEMP=115
	STEPTEMP=3

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for temp in $(seq $STARTTEMP $STEPTEMP $ENDTEMP); do 
		echo "About to change temperature to $temp, giving 5 minutes opportunity to cancel" 
		sleep 300
		sudo $RBC/setOmega $CCELLTEMP $temp

		sudo $RBC/scripts/repeatRbQuickPolarization.sh $NUMEQUILRUN "Equil runs for temp=$temp (from $STARTTEMP to $ENDTEMP in steps of $STEPTEMP), $COMMENTS"
		sudo $RBC/scripts/PolarizationScript.sh $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN $DETUNE "temp=$temp,  $COMMENTS"
	# TEMP LOOP DONE
	done 
	echo "Done with temp step run from $STARTTEMP to $ENDTEMP" | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
