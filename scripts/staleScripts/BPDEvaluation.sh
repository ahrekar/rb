#!/bin/bash

COMMENTS=$1

RBC=/home/pi/RbControl

$RBC/setProbeDetuning -35
$RBC/faradayRotation "-35 GHz Detuning, BPD measurement, $COMMENTS"

$RBC/setProbeDetuning -6
$RBC/faradayRotation "-6 GHz Detuning, BPD measurement, $COMMENTS"

$RBC/faradayScanBPD 0 117 1 "Full Scan, BPD measurement, $COMMENTS"

$RBC/faradayScan 0 117 1 "Full Scan, rotating HWP, $COMMENTS"
