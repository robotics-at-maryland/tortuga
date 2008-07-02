#!/bin/bash -l

cd ../..
. scripts/setenv

if [[ $1 == 'clean' ]];then
	scons --clean -Q
elif [[ $1 == '' ]];then
	scons with_features=sonar -Q -j3
else
	echo Unknown build command.
	exit 1
fi
