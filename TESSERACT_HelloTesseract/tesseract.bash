#!/bin/bash
# Tested on ubuntu virtual machine 16.04.1 LTS xenial

#  REFERENCES
# https://askubuntu.com/a/689757
# https://askubuntu.com/a/430722
# https://www.howtoinstall.co/en/ubuntu/utopic/autoconf-archive
# https://askubuntu.com/q/226148
# https://github.com/beakerbrowser/beaker/issues/54
# http://stackoverflow.com/a/22801937/1887602
# https://www.cyberciti.biz/faq/set-environment-variable-linux/

#PRE REQUISITES
sudo apt-get install -y build-essential
sudo apt-get install -y autotools-dev autoconf
sudo apt-get install -y autoconf-archive
sudo apt-get install -y libtool
sudo apt-get install -y pkg-config
sudo apt-get install -y libpng-dev
sudo apt-get install -y libjpeg-dev 
sudo apt-get install -y libtiff-dev
sudo apt-get install -y zlib1g-dev

#used only for the training module of OCR
sudo apt-get install -y libicu-dev
sudo apt-get install -y libpango1.0-dev libpango1.0-doc
sudo apt-get install -y libcairo2-dev libcairo2-doc

sudo apt-get update

# install leptonica
CWD=$(pwd)
git clone https://github.com/DanBloomberg/leptonica.git
sudo mv leptonica /opt/
sudo chown root:root -R /opt/leptonica
sudo chmod o+w -R /opt/leptonica
cd /opt/leptonica
./autobuild
./configure LDFLAGS="-L/usr/local/lib" CFLAGS="-I/usr/local/include"
make
sudo make install
sudo ldconfig

#install tesserak
cd $CWD
git clone https://github.com/tesseract-ocr/tesseract.git
sudo mv tesseract /opt/
sudo chown -R root:root /opt/tesseract
sudo chmod o+w -R /opt/tesseract
cd /opt/tesseract
./autogen.sh
./configure LDFLAGS="-L/usr/local/lib" CFLAGS="-I/usr/local/include"
make
sudo make install
sudo ldconfig

#install training modules for Tesserak
make training
sudo make training-install
sudo ldconfig

#install trained data for Tesserak. Mandatory if you want to use tesseract. You
#can get several other @ https://github.com/tesseract-ocr/tessdata
cd /usr/local/share/tessdata
sudo wget https://github.com/tesseract-ocr/tessdata/raw/master/eng.traineddata
sudo wget https://github.com/tesseract-ocr/tessdata/raw/master/ita.traineddata

export TESSDATA_PREFIX=/usr/local/share/tessdata/

#add environment variable mandatory in order for tesseract to work
sudo sed 's/exit 0/export TESSDATA_PREFIX=\/usr\/local\/share\/tessdata\/\nexit 0/g' -i /etc/rc.local


cd $CWD



