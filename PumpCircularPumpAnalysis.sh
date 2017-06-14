#!/bin/bash

sudo /home/pi/RbControl/setLaserFlag 1 1
sudo /home/pi/RbControl/RbAbsorbScan 0 1023 5 37.0 "Magnets at 65.4, 64.6. RbAbsorb Data, minimized transmission, pump off"
sudo /home/pi/RbControl/setLaserFlag 1 0
for i in $(seq 25 2 35); do 
	sudo /home/pi/RbControl/setWavePlate $i
    sudo /home/pi/RbControl/RbAbsorbScan 0 1023 5 37.0 "Magnets at 65.4, 64.6. QWP at step $i, RbAbsorb Data, minimized transmission, pump on"
done
