#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo "usage: ./twistTwoMotorsAndMonitorPD.sh <MIN STEP POS> <MAX STEP POS> <STEP SIZE>"
else
	MIN=$1
	MAX=$2
	STEP=$3
	RBC=/home/pi/RbControl

	for pos in $(seq $MIN $STEP $MAX); do
		echo "Running for QWP pos = $pos out of $MAX"
		echo "Pausing for 5 sec to give chance to cancel"
		sleep 5
		$RBC/setWavePlate $pos
		$RBC/getAngleBPD
		#$RBC/faradayRotation "Evaluating pump polarization vs. QWP position, pos=$pos"
	done 
