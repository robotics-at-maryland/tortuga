#! /bin/bash

# Change this if line if script name changes
SCRIPT_NAME=clean.sh
# Grab the full path to the script and then pull just the directory off it
SCRIPT_PATH=`pwd`/$0
SCRIPT_DIR=${SCRIPT_PATH:0:$(( ${#SCRIPT_PATH} - ${#SCRIPT_NAME}))}

# Remove everything SVN is ignoring
rm -rf `svn propget svn:ignore $SCRIPT_DIR`
