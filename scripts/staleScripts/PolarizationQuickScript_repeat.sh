#!/bin/bash

if [ "$#" -ne 2 ]; then 
	echo "usage: sudo ./PolarizationScript_repeat.sh <runs> <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="-240"
	N2OFFSET="100"
	ONED="1.5"
	TWOA="0"
	POLARIZATIONHEOFFSET=110
	HEOFFSET=-100
	CURRENTSCALE=6
	SCANRANGE=45
	STEPSIZE=24
	DWELL=1
	NUMRUN=1
	DETUNE=1.5
	RUNS=$1
	COMMENTS=$2

	AOUTS="110.7"

    PUMP=1
    PROBE=0

	QUICKPOLDWELL=3

    BLOCKED=1
    UNBLOCKED=0

	for run in $(seq $RUNS); do 
		echo "Measuring polarization."
		sudo $RBC/scripts/PolarizationQuickScript.sh $FILBIAS $N2OFFSET $ONED $TWOA $HEOFFSET $CURRENTSCALE $QUICKPOLDWELL $AOUTS $NUMRUN $DETUNE  "detune->$DETUNE, Run->$run, totalRuns->$RUNS, $COMMENTS"

		#echo "Waiting 5 min to step temperature and make next measurement"
		#sleep 300
		echo "Waiting 6 minutes to do next run"
		sleep 360
	done 
fi
