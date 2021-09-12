RBC=/home/pi/RbControl

DETECTOR=cb
SAMPLE=L
BIAS=-9
RUN=2

sudo $RBC/setPumpDetuning 1.5
sleep 60

sudo $RBC/setPumpDetuning 1.5
sudo $RBC/asymmetry 10 10 "detector->$DETECTOR, detuning->1.5, bias->$BIAS, run->$RUN, sample->$SAMPLE"
#sudo $RBC/setPumpDetuning 12
#sudo $RBC/asymmetry 10 10 "detector->$DETECTOR, detuning->12, bias->$BIAS, run->$RUN, sample->$SAMPLE"
echo "The two files are:"
ls /home/pi/RbData/$(date +%Y-%m-%d)/ | tail -n 2
sudo $RBC/setPumpDetuning 1.5
