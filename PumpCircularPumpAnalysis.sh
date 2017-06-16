#!/bin/bash

for i in $(seq 44 1 48); do 
	$RBC/setWavePlate $i
    $RBC/faradayRotation 0 0 0 "Wave plate set at $i, Finding circular, should be one with least variation in intensity over revolution."
done
