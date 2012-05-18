#!/bin/bash
# TODO: Eventually this should be a python script that simply renames directories and should not use symbolic links

if [ $# -ne 1 ]; then
    echo "Swaps between qor mods."
    echo "Usage: ./mod.sh {mod}"
    echo "Example: ./mod.sh demo"
    exit 1
fi

if [ ! -e "src/mod_$1" ]; then
    echo "Mod specified does not exist"
    exit 1
fi

mod="mod_$1"

rm -rf src/game 2>/dev/null
pushd src > /dev/null
ln -s $mod game
popd > /dev/null
