#!/bin/bash

# Usage:
#
#		sudo ./takePicture.sh 

if [ "$#" -ne 0 ]; then
	echo "usage: sudo ./takePicture.sh"
else
	# Take a picture of the chamber
	FILENAME=$(date +'%Y-%m-%d_%H%M%S').jpg
	mkdir -p /home/pi/RbPictures/$DATE
	# take Picture takes the filename and a subfolder to store the 
	# file in within the RbPicture folder
	ssh -i /home/pi/.ssh/id_rsa pi@irpi "/home/pi/karlCode/takeRemotePicture.sh $FILENAME"
fi
