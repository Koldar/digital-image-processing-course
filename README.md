# digital-image-processing-course

A repository containing all the code about digital image processing UNIBS course

# COURSE ENVIRONMENT

This course exploits openCV on a ubuntu machine. The following contains information about my environment:

 - `uname -a`: Linux koldar 4.4.0-64-generic #85-Ubuntu SMP Mon Feb 20 11:50:30 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux
 - `/proc/cpuinfo`: Dual core 64bit, , Intel(R) Core(TM) i5-3230M CPU @ 2.60GHz,
 - `/proc/meminfo | grep MemTotal`: MemTotal:        4046500 kB;
 - `lsb_release -irc`: Distributor ID:	Ubuntu, Release:	16.04, Codename:	xenial;
 
# INSTALLATION

OpenCV has been installed with the following commands (we used the installation process available at
http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html.

```
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev

cd /usr/local
sudo mkdir opencv
sudo chmod o+rw opencv/
cd opencv/
git clone https://github.com/opencv/opencv.git
mkdir build
cd build/
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
make
sudo make install
```



