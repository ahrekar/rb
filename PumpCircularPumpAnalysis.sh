#!/bin/bash

for i in $(seq 0 10 350); do 
	sudo /home/pi/RbControl/setWavePlate $i
    /home/pi/RbControl/faradayRotation 37 65.4 64.5 "QWP at step $i, fourierData"
done
