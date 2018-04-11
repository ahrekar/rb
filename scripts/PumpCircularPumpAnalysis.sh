#!/bin/bash

RBC=/home/pi/RbControl

for i in $(seq 0 3 349); do 
	echo "Waiting for 5 seconds before starting next run..."
	sleep 5
	$RBC/setWavePlate $i
    $RBC/faradayRotation "Wave plate in position $i, finding max polarization"
done
