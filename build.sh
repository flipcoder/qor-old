#!/bin/bash

rm bin/qor > /dev/null

# generate project/make files
premake4 vs2010 > /dev/null
premake4 gmake > /dev/null

# run native make file
make 2>&1

