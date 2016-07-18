#!/bin/bash
#
# This is a script to facilitate the running
# of the setProbeLaser script. The goal was
# to see how long it takes the laser to "settle"
# after the wavelength has been adjusted. 
# The script is run, and then I sat next to the 
# Wavemeter and measured the wavelength every 
# 30 seconds.

AOUT=(0 100 200 300 400 500 600 700 800 900 1000)

for i in ${AOUT[@]}; do
	sudo /home/pi/RbControl/setProbeLaser ${i}
	echo "AOUT set to ${i}."
	echo "Time is:"
	for i in `seq 1 6`; do
		echo "Take reading! Time is:"
		date
		sleep 30
	done
done
