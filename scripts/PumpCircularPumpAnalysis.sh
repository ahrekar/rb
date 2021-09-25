#!/bin/bash

PUMP=1
PROBE=0

BLOCKED=1
UNBLOCKED=0
RBC=/home/pi/RbControl

echo "setting the probe laser to a proper detuning"
sudo $RBC/setProbeDetuning -30

echo "setting the pump laser to a proper detuning"
sudo $RBC/setPumpDetuning -1.5

echo "setting the flip mirror to monitor pump beam"
sudo $RBC/toggleFlipMirror

#set laser flag to unblock the probe beam.
echo "Unblocking probe Beam."
$RBC/setLaserFlag $PROBE $UNBLOCKED

#set laser flag to unblock the pump beam.
echo "unBlocking Pump Beam."
$RBC/setLaserFlag $PUMP $UNBLOCKED

for i in $(seq 150 1 170); do 
	echo "setting the pump laser to a proper detuning"
	sudo $RBC/setPumpDetuning -1.5
	echo "setting the probe laser to a proper detuning"
	sudo $RBC/setProbeDetuning -30
	echo "Waiting for 5 seconds before starting next run..."
	sleep 5
	$RBC/setWavePlate $i
    sudo $RBC/faradayRotation "wavePlatePos->$i, searching for polarization reversal"
done

for i in $(seq 62 1 82); do 
	echo "setting the pump laser to a proper detuning"
	sudo $RBC/setPumpDetuning -1.5
	echo "setting the probe laser to a proper detuning"
	sudo $RBC/setProbeDetuning -30
	echo "Waiting for 5 seconds before starting next run..."
	sleep 5
	$RBC/setWavePlate $i
    sudo $RBC/faradayRotation "wavePlatePos->$i, searching for polarization reversal"
done
