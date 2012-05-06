#!/bin/bash
# TODO: Eventually this should be a python script that simply renames directories and should not use symbolic links

if [ $# -ne 1 ]; then
    echo "Swaps between qor mods."
    echo "Usage: ./mod.sh {mod}"
    echo "Example: ./mod.sh demo"
    exit
fi

if [ -e "mod_$1" ]; then
    echo "Mod specified does not exist"
fi

mod="mod_$1"

rm -rf src/game 2>/dev/null
ln -s $mod src/game
