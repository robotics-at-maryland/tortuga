#!/bin/bash

if [ ! -n "$1" ]; then
    echo "Must give a configuration file."
else
    # Vehicle has the code in ram_code
    cd ~/ram_code
    . scripts/setenv
    ./scripts/reload_cameras.sh
    lcdshow -unsafe
    python tools/oci/src/main.py -c $1
    killall -9 python
fi
