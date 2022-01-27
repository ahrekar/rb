#!/bin/bash

RBC=/home/pi/RbControl

sudo $RBC/scripts/RbQuickPolarizationScript.sh "Auto running while reservoir warms"
sudo $RBC/scripts/pictureDifferentPumpLights.sh "autoRunResWarm"
