#!/bin/bash

sudo apt-get install patch build-essential libreadline-dev libncurses-dev libusb-dev libftdi-dev automake autoconf bison flex
sudo apt-get build-dep avrdude avrdude-doc
mkdir avrdude
cd avrdude
wget http://www.ftdichip.com/Drivers/D2XX/Linux/libftd2xx0.4.16_x86_64.tar.gz
wget http://download.savannah.gnu.org/releases-noredirect/avrdude/avrdude-5.10.tar.gz
for i in 8 7 6 5 4 3 2 1 0; do wget -O patch-$i.diff http://savannah.nongnu.org/patch/download.php?file_id=1851$i; done
tar xzf avrdude-5.10.tar.gz
tar xzf libftd2xx*.tar.gz
cd avrdude-5.10
for file in ../patch-*.diff; do patch -p0 < $file; done
cp ../libftd2xx*/static_lib/* .
cp ../libftd2xx*/*.h .
cp ../libftd2xx*/*.cfg .
./configure CFLAGS="-g -O2 -DSUPPORT_FT245R" LIBS="./libftd2xx.a.0.4.16 -lpthread -ldl -lrt"
make
sudo make install
cd ../../

./ctohw.sh
