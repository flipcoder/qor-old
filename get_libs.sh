#!/bin/bash

if [ $# -eq 0 ]; then
	echo Start this script with parameters to how you how normally install a package for your distro.
	echo Example: ./get_libs.sh
	echo Note: Do not include the sudo command.
	exit
fi

echo This script will use \`apt-get\` and wget to obtain the packages necessary for development.
echo Press a key...
read

sudo apt-get install unrar
sudo apt-get install cmake
sudo apt-get install `cat third_party/lib_names.txt`

mkdir temp
pushd temp > /dev/null

# install newton dynamics
if [ -d newton-dynamics-2.32 ]; then
	echo Found Newton directory, skipping download...
else
	if [ -f newton-dynamics-2.32.rar ]; then
		echo Found RAR file, skipping download...
	else
		wget http://newton-dynamics.googlecode.com/files/newton-dynamics-2.32.rar
	fi

	unrar x newton-dynamics-2.32.rar
fi

pushd newton-dynamics-2.32 >/dev/null

if [ `uname -m | grep 64$` ]; then
	pushd coreLibrary_200/projets/linux64 >/dev/null
else 
	pushd coreLibrary_200/projets/linux32 >/dev/null
fi

make
sudo cp libNewton.a /usr/local/lib/
popd

pushd coreLibrary_200/source/ >/dev/null
cp -r * ../../../../third_party/include/
popd
popd

# install assimp
#if [ `cat /etc/apt/sources.list | grep openrave` ]; then
#    echo assimp repo already added to sources.list...
#else
#    echo Making a back-up of sources.list...
#    sudo cp /etc/apt/sources.list /etc/apt/sources.list.bak
#    echo "deb http://ppa.launchpad.net/openrave/release/ubuntu natty main" >> /etc/apt/sources.list
#    echo "deb-src http://ppa.launchpad.net/openrave/release/ubuntu natty main" >> /etc/apt/sources.list
#fi

#sudo apt-get install assimp-dev

echo Done! Press a key to exit...
read

