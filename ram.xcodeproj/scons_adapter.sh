#!/bin/bash -l

. scripts/setenv
if [[ $1 == 'clean' ]];then
    scons --clean -Q
elif [[ $1 == '' ]];then
    scons -Q -j3
else
    scons -Q -j3 with_features=$1
fi
