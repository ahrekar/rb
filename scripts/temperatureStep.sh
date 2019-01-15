#!\bin/bash

for i in $(seq 120 2 150); do 
	sudo  /home/pi/RbControl/setOmega 175 $i
	sleep 1200
done

