#!/bin/bash

if [ $# -eq 0 ]; then
	echo Start this script with parameters to how you how normally install a package for your distro.
	echo Example: ./get_libs.sh apt-get install
	echo Note: Do not include the sudo command.
	exit
fi

echo Make sure you have the Source Code option enabled in your distros repository.  In Ubuntu/Debain, you can do this through synaptic.
echo Press a key...
read

sudo $@ unrar
sudo $@ `cat third_party/lib_names.txt`

mkdir temp
pushd temp > /dev/null

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

echo Done! Press a key to exit...
read

