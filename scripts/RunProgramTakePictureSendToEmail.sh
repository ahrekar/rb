#!/bin/bash

RBC=/home/pi/RbControl

PROBE=0
PUMP=1

UNBLOCKED=0
BLOCKED=1


$RBC/setLaserFlag $PROBE $UNBLOCKED
$RBC/setLaserFlag $PUMP $UNBLOCKED
$RBC/scripts/takePicture.sh "RbAutoPic"

$RBC/setLaserFlag $PROBE $UNBLOCKED
$RBC/setLaserFlag $PUMP $BLOCKED
$RBC/RbAbsorbScan 45 65 .2 "RbAutoScan"

$RBC/setLaserFlag $PROBE $UNBLOCKED
$RBC/setLaserFlag $PUMP $UNBLOCKED
$RBC/scripts/sendLastDataImageAndPhotoToEmail.sh
 	
