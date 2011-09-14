#!/bin/bash

rm bin/qor
pushd build > /dev/null
cmake . > /dev/null
make 2>&1
popd
