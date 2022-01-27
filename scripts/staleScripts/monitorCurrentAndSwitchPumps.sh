#!/bin/bash

source LoadWaveplatePositions.sh

echo "Blocking probe beam..."
$RBC/setLaserFlag $PROBE $BLOCKED

echo "Blocking pump beam..."
$RBC/setLaserFlag $PUMP $BLOCKED
echo "No pump..."
$RBC/monitorCountsAndCurrent 1 1 6 "No pump"

echo "Unblocking pump beam..."
$RBC/setLaserFlag $PUMP $UNBLOCKED

echo "Setting pump to Pi..."
$RBC/setWavePlate $PIPOS
echo "Pi Polarized light..."
$RBC/monitorCountsAndCurrent 1 1 6 "Pi pump"

echo "Setting pump to S+..."
$RBC/setWavePlate $SPLUSPOS
echo "S+ light..."
$RBC/monitorCountsAndCurrent 1 1 6 "S+ pump"

echo "Setting pump to S-..."
$RBC/setWavePlate $SMINUSPOS
echo "S- light..."
$RBC/monitorCountsAndCurrent 1 1 6 "S- pump"

echo "Unblocking probe beam..."
$RBC/setLaserFlag $PROBE $UNBLOCKED

