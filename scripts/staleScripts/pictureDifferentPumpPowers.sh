#!/bin/bash

RBC=/home/pi/RbControl

sudo $RBC/setTACurrent 1500
sleep 3
sudo $RBC/setTACurrent 2500
sleep 3
sudo $RBC/scripts/pictureDifferentPumpLights.sh
sudo $RBC/setTACurrent 750
sudo $RBC/scripts/pictureDifferentPumpLights.sh
