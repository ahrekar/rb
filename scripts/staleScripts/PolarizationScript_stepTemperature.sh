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
	CURRENTSCALE=7
	SCANRANGE=45
	STEPSIZE=24
	DWELL=1
	NUMRUN=1
	DETUNE=1.5
	COMMENTS=$1
	CCELLTEMP=200
	STARTTEMP=159
	ENDTEMP=178
	STEPTEMP=1

    PUMP=1
    PROBE=0

	QUICKPOLDWELL=3

    BLOCKED=1
    UNBLOCKED=0

	for temp in $(seq $STARTTEMP $STEPTEMP $ENDTEMP); do 
		echo "Measuring polarization."
		sudo $RBC/scripts/PolarizationScript.sh $FILBIAS $N2OFFSET $ONED $TWOA $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN $detune "detune->$detune, $COMMENTS"

		echo "About to change temperature to $temp, giving 10 s opportunity to cancel" 
		sleep 10
		sudo $RBC/setOmega $CCELLTEMP $temp
		echo  '$RBC/setOmega $CCELLTEMP $temp'

		echo "Waiting 5 min to step temperature and make next measurement"
		sleep 300
		#QUICKPOLRUNS=3
		#for i in $(seq $QUICKPOLRUNS); do
		#	sudo $RBC/scripts/PolarizationQuickScript.sh $FILBIAS $N2OFFSET $ONED $TWOA $POLARIZATIONHEOFFSET $CURRENTSCALE $QUICKPOLDWELL $POLARIZATIONHEOFFSET 1 1.5 "quickPolarizations between, full pols, run $i/$QUICKPOLRUNS"
		#	echo "Waiting 6 min to make another quick pol measurement."
		#	sleep 360
		#done
	done 
fi
