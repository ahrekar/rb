#!/bin/bash

# This is a script to analyze the effect that the magnetic
# field has on the plane of polarization. It will do the 
# faraday rotation analysis for a single detuning of the
# probe laser. Data will be taken for both when the pump
# entering the chamber and when it is not.

REVOLUTIONS=(1 2 3 4)
NDATAPOINTS=(4 8 12 16 24 30 75 200)
for i in "${REVOLUTIONS[@]}"; do 
	for j in "${NDATAPOINTS[@]}"; do
		sudo ./stepperMotorDiagnose $j $i
	done
done
