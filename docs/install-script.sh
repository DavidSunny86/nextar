add-apt-repository ppa:ubuntu-toolchain-r/test
apt-get update; 
apt-get -y install libx11-dev mesa-common-dev freeglut3-dev zlib1g-dev libxi-dev libxrandr-dev libxi-dev libjpeg-dev libpng-dev libopenexr-dev openexr-viewers flex bison++
apt-get update
apt-get -y upgrade
apt-get -y dist-upgrade

