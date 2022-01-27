#!/bin/bash

source LoadWaveplatePositions.sh

echo "Blocking probe beam..."
$RBC/setLaserFlag $PROBE $BLOCKED

echo "Blocking pump beam..."
$RBC/setLaserFlag $PUMP $BLOCKED
echo "No pump..."
$RBC/monitorCountsAndCurrent 1 60 6 "No pump"
$RBC/monitorCountsAndCurrent 1 60 6 "No pump"
$RBC/monitorCountsAndCurrent 1 60 6 "No pump"
$RBC/monitorCountsAndCurrent 1 60 6 "No pump"

