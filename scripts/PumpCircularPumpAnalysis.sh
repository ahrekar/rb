#!/bin/bash

PUMP=1
PROBE=0

BLOCKED=1
UNBLOCKED=0
RBC=/home/pi/RbControl

echo "setting the probe laser to a proper detuning"
sudo $RBC/setProbeLaser 25

echo "setting the flip mirror to monitor probe beam"
sudo $RBC/setMirror $PROBE

#set laser flag to unblock the probe beam.
echo "Unblocking probe Beam."
$RBC/setLaserFlag $PROBE $UNBLOCKED

#set laser flag to unblock the pump beam.
echo "Blocking Pump Beam."
$RBC/setLaserFlag $PUMP $UNBLOCKED

for i in $(seq 0 3 349); do 
	echo "Waiting for 5 seconds before starting next run..."
	sleep 5
	$RBC/setWavePlate $i
    $RBC/faradayRotation "Wave plate in position $i, finding max polarization"
done
