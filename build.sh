#!/bin/bash

rm bin/qor
premake4 gmake > /dev/null
pushd build > /dev/null
make 2>&1
popd
