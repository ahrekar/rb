#!/bin/bash

RBC=/home/pi/RbControl

if [ "$#" -ne 5 ]; then
	echo "usage: ./repeatElectronPolarization.sh <HE target Voltage> "
	echo "                                       <dwell time> "
	echo "                                       <order of magnitude of current> "
	echo "                                       <number of runs> "
	echo "                                       <detune (1.5 (GHz) is max)>"
	echo "                                       <comments>"
	echo "                                                  "
else
	AOUT=$1
	DWELL=$2
	EXP=$3
	NUMRUN=$4
	DETUNE=$5
	COMMENTS=$6
	for i in $(seq $NUMRUN); do
		echo "Run number ${i}/$NUMRUN, pausing for 3 seconds to allow the opportunity to cancel"
		sleep 3
		sudo $RBC/scripts/ElectronPolarizationScript.sh $AOUT $DWELL $EXP $DETUNE"$COMMENTS, Run${i}/$NUMRUN"
	done
fi
