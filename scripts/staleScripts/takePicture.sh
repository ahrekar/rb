#!/bin/bash

# Usage:
#
#		sudo ./takePicture.sh 

if [ "$#" -lt 1 ]; then
	echo "usage: sudo ./takePicture.sh <pictureID>"
	echo "usage: sudo ./takePicture.sh <pictureID> <Shutter Speed> <ISO>"
else
	if [ "$#" -eq 1 ]; then
		PICTUREID=$1
		# Take a picture of the chamber
		FILENAME="$(date +'%Y-%m-%d_%H%M%S')$PICTUREID"
		mkdir -p /home/pi/RbPictures/$DATE
		# take Picture takes the filename and a subfolder to store the 
		# file in within the RbPicture folder
		ssh -i /home/pi/.ssh/id_rsa pi@irpi "/home/pi/karlCode/takeRemotePicture.sh $FILENAME"
	elif [ "$#" -eq 3 ]; then
		PICTUREID=$1
		SS=$2
		ISO=$3
		# Take a picture of the chamber
		FILENAME="$(date +'%Y-%m-%d_%H%M%S')$PICTUREID"
		mkdir -p /home/pi/RbPictures/$DATE
		# take Picture takes the filename and a subfolder to store the 
		# file in within the RbPicture folder
		ssh -i /home/pi/.ssh/id_rsa pi@irpi "/home/pi/karlCode/takeRemotePicture.sh $SS $ISO $FILENAME"
	fi
fi
