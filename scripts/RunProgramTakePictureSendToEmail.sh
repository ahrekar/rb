#!/bin/bash

RBC=/home/pi/RbControl

PROBE=0
PUMP=1

UNBLOCKED=0
BLOCKED=1


#$RBC/setLaserFlag $PROBE $UNBLOCKED
#$RBC/setLaserFlag $PUMP $UNBLOCKED
#$RBC/scripts/takePicture.sh "RbAutoPic"

$RBC/setLaserFlag $PROBE $UNBLOCKED
$RBC/setLaserFlag $PUMP $UNBLOCKED
$RBC/faradayScan "Laser On, warming cell"
$RBC/setLaserFlag $PUMP $BLOCKED
$RBC/faradayScan "Laser Off, warming cell"
#$RBC/RbAbsorbScan 33 67 .3 "Number density settles."

$RBC/setLaserFlag $PROBE $UNBLOCKED
$RBC/setLaserFlag $PUMP $UNBLOCKED
#$RBC/scripts/sendLastDataImageToEmail.sh
