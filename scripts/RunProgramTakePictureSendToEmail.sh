#!/bin/bash

RBC=/home/pi/RbControl

PROBE=0
PUMP=1

UNBLOCKED=0
BLOCKED=1


$RBC/setLaserFlag $PROBE $UNBLOCKED
$RBC/setLaserFlag $PUMP $UNBLOCKED
$RBC/scripts/takePictureAndSendToEmail.sh "RbAutoPic"

$RBC/setLaserFlag $PROBE $UNBLOCKED
$RBC/setLaserFlag $PUMP $BLOCKED
#$RBC/faradayScan "Laser On, warming cell"
#$RBC/setLaserFlag $PUMP $BLOCKED
#$RBC/faradayScan "Laser Off, warming cell"
sudo $RBC/RbAbsorbScan 33 67 .3 "Cell cooling"

#$RBC/setLaserFlag $PROBE $UNBLOCKED
#$RBC/setLaserFlag $PUMP $UNBLOCKED
#$RBC/scripts/sendLastDataImageToEmail.sh
