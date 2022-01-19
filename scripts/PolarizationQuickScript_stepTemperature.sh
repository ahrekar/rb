#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="-240"
	N2OFFSET="100"
	ONED="1.5"
	TWOA="0"
	POLARIZATIONHEOFFSET=110
	HEOFFSET=-100
	CURRENTSCALE=6
	NUMRUN=1
	DETUNE=1.5
	COMMENTS=$1
	CCELLTEMP=200
	STARTTEMP=194
	ENDTEMP=196
	STEPTEMP=2

	AOUTS="110.7"

    PUMP=1
    PROBE=0

	QUICKPOLDWELL=3

    BLOCKED=1
    UNBLOCKED=0

	for temp in $(seq $STARTTEMP $STEPTEMP $ENDTEMP); do 
		echo "Measuring polarization."
		sudo $RBC/scripts/PolarizationQuickScript.sh $FILBIAS $N2OFFSET $ONED $TWOA $HEOFFSET $CURRENTSCALE $QUICKPOLDWELL $AOUTS $NUMRUN $DETUNE  "detune->$DETUNE, resTemp->$temp, $COMMENTS"

		echo "About to change temperature to $temp, giving 10 s opportunity to cancel" 
		sleep 10
		sudo $RBC/setOmega $CCELLTEMP $temp
		echo  '$RBC/setOmega $CCELLTEMP $temp'

		#echo "Waiting 5 min to step temperature and make next measurement"
		#sleep 300
		echo "Waiting 6 min to step temperature and make next measurement"
		sleep 360
	done 
fi
