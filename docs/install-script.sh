add-apt-repository ppa:ubuntu-toolchain-r/test
apt-get update; 
update-alternatives --remove-all gcc 
update-alternatives --remove-all g++
apt-get -y install libx11-dev mesa-common-dev freeglut3-dev zlib1g-dev gcc-4.9 g++-4.9
apt-get -y install libxrandr-dev
apt-get -y install libxi-dev
apt-get -y install libjpeg-dev libpng-dev
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 20
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 20
update-alternatives --config gcc
update-alternatives --config g++
apt-get update
apt-get -y upgrade
apt-get -y dist-upgrade

