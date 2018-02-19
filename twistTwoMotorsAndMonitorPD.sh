#!/bin/bash

MIN=70
MAX=80
STEP=1
RBC=/home/pi/RbControl

for pos in $(seq $MIN $STEP $MAX); do
	echo "Running for QWP pos = $pos out of $MAX"
	echo "Pausing for 5 sec to give chance to cancel"
	sleep 5
	$RBC/setWavePlate $pos
	$RBC/faradayRotation "Evaluating pump polarization vs. QWP position, pos=$pos"
done 
