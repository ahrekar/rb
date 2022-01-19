RBC=/home/pi/RbControl

DETECTOR=cb
SAMPLE=D
BIAS=-9
RUN=3
COMMENTS="overallRun->14"

sudo $RBC/setLaserFlag 1 0
sudo $RBC/setPumpDetuning 1.5
sleep 30

sudo $RBC/setPumpDetuning 1.5
sudo $RBC/asymmetry 10 10 "detector->$DETECTOR, detuning->1.5, bias->$BIAS, run->$RUN, sample->$SAMPLE, $COMMENTS"
#sudo $RBC/setPumpDetuning 12
#sudo $RBC/asymmetry 10 10 "detector->$DETECTOR, detuning->12, bias->$BIAS, run->$RUN, sample->$SAMPLE, $COMMENTS"
echo "The two files are:"
ls /home/pi/RbData/$(date +%Y-%m-%d)/ | tail -n 2
sudo $RBC/setLaserFlag 1 1
sudo $RBC/setPumpDetuning 1.5
