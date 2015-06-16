#!/bin/bash
#
# This is a script to facilitate the repeated running of the 
# diagnoseCounts script. 

FREQUENCIES=(30 100 300 1000 3000 10000 30000 100000)

for i in ${FREQUENCIES[@]}; do
	echo "Set the frequency to ${i} and press enter to continue"
	read enterpress
	sudo ./getcounts 10 ${i} >> countError.data
done
