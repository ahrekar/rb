#!/bin/bash
# Script to obtain polarization Data

# set QWP for s+ light
sudo ./setWavePlate 17
sudo ./faradayscan2 "$1" "$2" "$3" "$4 s+ pump"
sudo ./polarization 100 1 "$4 s+ pump"

# set QWP for s- light
sudo ./setWavePlate 105
sudo ./faradayscan2 "$1" "$2" "$3" "$4 s- pump"
sudo ./polarization 100 1 "$4 s- pump"
