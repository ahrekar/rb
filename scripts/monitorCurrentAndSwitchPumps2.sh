#!/bin/bash

PIPOS=74
SPLUSPOS=30
SMINUSPOS=118

PUMP=1
PROBE=0

BLOCKED=1
UNBLOCKED=0
RBC=/home/pi/RbControl

echo "Blocking probe beam..."
$RBC/setLaserFlag $PROBE $BLOCKED

echo "Blocking pump beam..."
$RBC/setLaserFlag $PUMP $BLOCKED
echo "No pump..."
$RBC/monitorCountsAndCurrent 1 60 6 "No pump"
$RBC/monitorCountsAndCurrent 1 60 6 "No pump"
$RBC/monitorCountsAndCurrent 1 60 6 "No pump"
$RBC/monitorCountsAndCurrent 1 60 6 "No pump"

