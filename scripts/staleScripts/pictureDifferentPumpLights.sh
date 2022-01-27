#!/bin/bash

PUMP=1
PROBE=0

BLOCKED=1
UNBLOCKED=0

RBC=/home/pi/RbControl

# With the probe laser at a voltage offset of 47.3, the probe laser will be on resonance at 345 AOUT, (377.1106 GHz)
sudo $RBC/setProbeLaser 45

echo "Unblocking both lasers..."
sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
sudo $RBC/setLaserFlag $PROBE $UNBLOCKED

$RBC/scripts/takePicture.sh "BOTHON_longExp" 5000000 100
$RBC/scripts/takePicture.sh "BOTHON_shortExp" 500000 100


echo "Blocking probe laser..."
sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
sudo $RBC/setLaserFlag $PROBE $BLOCKED

$RBC/scripts/takePicture.sh "PUMPON_longExp" 5000000 100
$RBC/scripts/takePicture.sh "PUMPON_shortExp" 500000 100

echo "Blocking pump laser..."
sudo $RBC/setLaserFlag $PUMP $BLOCKED
sudo $RBC/setLaserFlag $PROBE $UNBLOCKED

$RBC/scripts/takePicture.sh "PROBEON_longExp" 5000000 100

echo "Unblocking both lasers..."
sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
sudo $RBC/setLaserFlag $PROBE $UNBLOCKED
