#sudo apt install qtcreator libssl-dev git;
sudo apt install qt5-default;
git clone https://github.com/derekmolloy/ee513Assignment2;
git clone http://github.com/eclipse/paho.mqtt.c;
cd paho.mqtt.c/;
make;
sudo make install;
qmake --version;
qtcreator &;

