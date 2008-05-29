#!/bin/sh

if [[ $1 == 'clean' ]];then
	/opt/local/bin/scons --clean -Q
elif [[ $1 == '' ]];then
	/opt/local/bin/scons -Q -j3
else
	echo Unknown build command.
	exit 1
fi
