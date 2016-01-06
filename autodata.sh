#!/bin/bash
#
# USAGE:
#		
#		sudo ./autoData.sh "<comments>"

# First, scan for largest absorption peak.
sudo ./RbAbsorbScan 700 1000 10 "$1"

# Then do a faraday Scan.
sudo ./faradayscan2 100 601 100 "$1"

# Then measure the polarization with different laser
# settings.
echo "polarization with laser 30ev \n"
sudo ./polarization 2 20 1 0 1 "$1"

echo "polarization without laser 30ev \n"
sudo ./polarization 2 20 1 0 0 "$1"

echo "polarization with laser 15ev \n"
sudo ./polarization 2 20 1 500 1 "$1"

# Reset the output to zero just to be sure.
sudo ./setHP3617 0

# Reset the motor to its home location.
sudo ./homemotor
