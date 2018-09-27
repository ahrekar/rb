sudo apt-get install vim
sudo apt-get install emacs
sudo apt-get install git
git clone https://github.com/ahrendsen/picam
sudo apt-get install fswebcam
touch /home/pi/.startCount
git clone https://github.com/andrewfrommelbourne/dmxwebcam
sudo apt-get install libbsd-dev
mkdir build
cd build/
sudo apt-get install cmake
cmake ..
git clone https://github.com/ahrendsen/RbControl
sudo apt-get install wiringPi
git clone git://git.drogon.net/wiringPi
cd wiringPi/
./build 
sudo raspi-config
export EDITOR=/usr/bin/vim
git config --global user.email "karl.ahrendsen@gmail.com"
git config --global user.name "Karl Ahrendsen"
