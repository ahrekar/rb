#!/bin/bash

RBC=/home/pi/RbControl

echo "$($RBC/interfacing/systemStatus)" | mutt -a $(ls /home/pi/RbData/2018-03-10/*.png | tail -n 1) -- karl@huskers.unl.edu
