#!/bin/bash

# script to take faraday scan data

#

#set laser flag to block the pump beam
sudo ./setLaserFlag 3
#scan
sudo ./faradayscan2 "$1" "$2" "$3" "$4 no beam"

# set s laser flag to pass the beam
sudo ./setLaserFlag 0
# set QWP for s+ light
sudo ./setWavePlate 18
sudo ./faradayscan2 "$1" "$2" "$3" "$4 s+ pump"

# set QWP for s- light
sudo ./setWavePlate 105
sudo ./faradayscan2 "$1" "$2" "$3" "$4 s- pump"

