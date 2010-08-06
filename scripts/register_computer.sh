#!/bin/bash
# This script will be called by another script when appropriate!
# DO NOT USE THIS SCRIPT ON YOUR OWN!

if [ ! -n "$3" ]; then
    echo "Do NOT run this script, jackass."
else
    mkdir -p .ssh
    echo $1 $2 $3 | cat >> .ssh/authorized_keys2
fi
